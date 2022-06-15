#include "SQLDatabaseStorage.h"
#include "Mediator.h"
#include <string>
#include <regex>
#include "RppUtils.hpp"
#include "ConfigEnums.h"
#include <config.h>

/// @brief reads input from input file, unless EOF has been reached
#define READ_LINE(p_inputFileStream, p_string)                  \
    if (p_inputFileStream.eof())                                \
    {                                                           \
        p_inputFileStream.close();                              \
        THROW_RPP_EXCEPTION("Reached end of file prematurely"); \
    }                                                           \
    std::getline(p_inputFileStream, p_string);

#define LOG_MYSQL_EXCEPTION(p_except)                               \
    std::cerr << "[MYSQL] ERROR: SQLException in " << __FILE__;     \
    std::cerr << "(" << __FUNCTION__ << ")" << std::endl;           \
    std::cerr << "ERROR: " << p_except.what();                      \
    std::cerr << " (MySQL error code: " << p_except.getErrorCode(); \
    std::cerr << ", SQLState: " << p_except.getSQLState() << " )" << std::endl

/// @brief executes sql statement
#define EXECUTE(p_sql) \
    m_statement->execute(p_sql)

/// @brief create sql insert ignore statement
#define INSERT_IGNORE_INTO(p_tableName, p_headers, p_values) \
    "INSERT IGNORE INTO " p_tableName "(" p_headers ") VALUES (" + p_values + ");"

/// @brief create sql insert statement
#define INSERT_INTO(p_tableName, p_headers, p_values) \
    "INSERT INTO " p_tableName "(" p_headers ") VALUES (" + p_values + ");"

#define GET_INT_FROM_QUERY(p_int, p_sql)            \
    m_resultSet = m_statement->executeQuery(p_sql); \
    while (m_resultSet->next()) p_int = m_resultSet->getInt(1)

/// @brief Executes a query and retrieves the integer result
#define GET_INT_FROM_PREP_QUERY(p_int, p_prepStmt) \
    m_resultSet = p_prepStmt->executeQuery();      \
    delete p_prepStmt;                             \
    while (m_resultSet->next()) p_int = m_resultSet->getInt(1)

#define STORE_ID_TRIGGER_NAME "store_id_trigger"

/// @brief The constructor of the SQL database storage, defaults to storing all data.
SQLDatabaseStorage::SQLDatabaseStorage()
    : SQLDatabaseStorage({true, true, true}){};

/// @brief The parameterized constructor of the SQL database storage
/// @param p_dataToStore The data to actually store in the database
SQLDatabaseStorage::SQLDatabaseStorage(tDataToStore p_dataToStore)
    : m_dataToStore(p_dataToStore)
{
    m_driver = nullptr;
    m_connection = nullptr;
    m_statement = nullptr;
    m_resultSet = nullptr;
}

/// @brief Connect to the specified database and set session variables.
///        Initialise the database with the proper tables if they don't exist yet.
/// @param p_dbSettings Struct containing all settings for a database connection
/// @return             Returns true if connection to database has been made, false otherwise
bool SQLDatabaseStorage::OpenDatabase(DatabaseSettings p_dbSettings)
{
    // Initialise SQL driver
    m_driver = sql::mysql::get_mysql_driver_instance();

    // Set connection options, and connect to the database
    sql::ConnectOptionsMap connection_properties;
    std::string address = p_dbSettings.Address;
    connection_properties["hostName"] = "tcp://" + address;
    connection_properties["userName"] = p_dbSettings.Username;
    connection_properties["password"] = p_dbSettings.Password;
    connection_properties["port"] = p_dbSettings.Port;
    connection_properties["OPT_RECONNECT"] = true;
    connection_properties["CLIENT_MULTI_STATEMENTS"] = false;
    connection_properties["CLIENT_LOCAL_FILES"] = true;
    connection_properties["sslEnforce"] = true;

    if (p_dbSettings.UseSSL) PutKeys(connection_properties, p_dbSettings);

    try
    {
        m_connection = m_driver->connect(connection_properties);
    }
    catch (sql::SQLException& e)
    {
        std::cerr << "Could not open database" << std::endl;
        LOG_MYSQL_EXCEPTION(e);

        return false;
    }

    // Create the database schema if this is a new schema. This has to be done before setting the schema on the connection.
    m_statement = m_connection->createStatement();
    std::string schema = p_dbSettings.Schema;
    EXECUTE("CREATE DATABASE IF NOT EXISTS " + schema);
    m_statement->close();
    delete m_statement;

    // Set the correct database schema and create (reusable statement)
    m_connection->setSchema(p_dbSettings.Schema);
    m_statement = m_connection->createStatement();

    // Our system will always use UTC times. Ensure the database knows this as well.
    EXECUTE("SET @@session.time_zone='+00:00';");

    // Enable loading local files (server side)
    EXECUTE("SET GLOBAL local_infile = true;");

    // Ensure all tables are created
    CreateTables();

    return true;
}

/// @brief Gets the keys for secure database connection in the data/certificates folder
///        and adds them to the connection properties
/// @param p_connectionProperties SQL connection properties to which the keys are added.
/// @param p_dbSettings           The database settings used to retrieve the certificate names.
void SQLDatabaseStorage::PutKeys(sql::ConnectOptionsMap& p_connectionProperties, DatabaseSettings p_dbSettings)
{
    p_connectionProperties["sslCA"] = p_dbSettings.CACertFilePath;
    p_connectionProperties["sslCert"] = p_dbSettings.PublicCertFilePath;
    p_connectionProperties["sslKey"] = p_dbSettings.PrivateCertFilePath;
}

/// @brief Executes sql create statements to create all tables
// TODO: update these to match the values of DataToStore in ConfigEnums
//  The current setup uses Gamestate for the CarData checkbox,
//  UserInput for "Human data" and Decisions for "Intervention data",
//  but there should be separate tables for environment, car and internal metadata.
void SQLDatabaseStorage::CreateTables()
{
    EXECUTE(
        "CREATE TABLE IF NOT EXISTS Participant (\n"
        "    participant_id VARCHAR(255) NOT NULL,\n"
        "    \n"
        "    CONSTRAINT participant_id_primary_key PRIMARY KEY (participant_id)\n"
        ")\n");

    EXECUTE(
        "CREATE TABLE IF NOT EXISTS Blackbox (\n"
        "    blackbox_id INT NOT NULL AUTO_INCREMENT,\n"
        "    filename    VARCHAR(255)    NOT NULL,\n"
        "    version     DATETIME(0)     NOT NULL,\n"
        "    name        VARCHAR(255)    NOT NULL,\n"
        "    \n"
        "    CONSTRAINT blackbox_id_primary_key PRIMARY KEY (blackbox_id),\n"
        "    \n"
        "    CONSTRAINT file_version UNIQUE (filename, version)\n"
        ")\n");

    EXECUTE(
        "CREATE TABLE IF NOT EXISTS Environment (\n"
        "    environment_id  INT             NOT NULL AUTO_INCREMENT,\n"
        "    filename        VARCHAR(255)    NOT NULL,\n"
        "    version         INT             NOT NULL,\n"
        "    name            VARCHAR(255)    NOT NULL,\n"
        "    \n"
        "    CONSTRAINT environment_id_primary_key PRIMARY KEY (environment_id),\n"
        "    \n"
        "    CONSTRAINT file_version UNIQUE (filename, version)\n"
        ")\n");

    EXECUTE(
        "CREATE TABLE IF NOT EXISTS Settings (\n"
        "    settings_id         INT  NOT NULL AUTO_INCREMENT,\n"
        "    intervention_mode   ENUM('Drive', 'Force', 'Shared', 'Suggest', 'Off') NOT NULL DEFAULT 'Off',\n"
        "    \n"
        "    CONSTRAINT settings_id_primary_key PRIMARY KEY (settings_id),\n"
        "    CONSTRAINT settings_unique UNIQUE (intervention_mode)\n"
        "    \n"
        ")");

    // Need to keep the enum column up-to-date, simply updating the create table will not be run since it exists.
    EXECUTE(
        "ALTER TABLE Settings MODIFY COLUMN intervention_mode "
        "ENUM('Drive', 'Force', 'Shared', 'Suggest', 'Off') NOT NULL DEFAULT 'Off';");

    EXECUTE(
        "CREATE TABLE IF NOT EXISTS Trial (\n"
        "    trial_id        INT         NOT NULL AUTO_INCREMENT,\n"
        "    trial_time      DATETIME(0) NOT NULL,\n"
        "    participant_id  VARCHAR(255) NOT NULL,\n"
        "    blackbox_id     INT         NOT NULL,\n"
        "    environment_id  INT         NOT NULL,\n"
        "    settings_id     INT         NOT NULL,\n"
        "    \n"
        "    CONSTRAINT trial_id_primary_key         PRIMARY KEY (trial_id),\n"
        "    CONSTRAINT participant_id_foreign_key   FOREIGN KEY (participant_id) REFERENCES Participant(participant_id),\n"
        "    CONSTRAINT blackbox_id_foreign_key      FOREIGN KEY (blackbox_id)    REFERENCES Blackbox(blackbox_id),\n"
        "    CONSTRAINT environment_id_foreign_key   FOREIGN KEY (environment_id) REFERENCES Environment(environment_id),\n"
        "    CONSTRAINT settings_id_foreign_key      FOREIGN KEY (settings_id)    REFERENCES Settings(settings_id)\n"
        ")");

    EXECUTE(
        "CREATE TABLE IF NOT EXISTS TimeStep (\n"
        "    trial_id        INT             NOT NULL,\n"
        "    tick            BIGINT UNSIGNED NOT NULL,\n"
        "    \n"
        "    CONSTRAINT trial_tick_primary_key    PRIMARY KEY (trial_id, tick),\n"
        "    CONSTRAINT trial_id_foreign_key      FOREIGN KEY (trial_id) REFERENCES Trial(trial_id)\n"
        ")");

    EXECUTE(
        "CREATE TABLE IF NOT EXISTS GameState (\n"
        "    game_state_id   BIGINT          NOT NULL AUTO_INCREMENT,\n"
        "    x               DOUBLE          NOT NULL,\n"
        "    y               DOUBLE          NOT NULL,\n"
        "    z               DOUBLE          NOT NULL,\n"
        "    direction_x     DOUBLE          NOT NULL,\n"
        "    direction_y     DOUBLE          NOT NULL,\n"
        "    direction_z     DOUBLE          NOT NULL,\n"
        "    speed           FLOAT           NOT NULL,\n"
        "    acceleration    FLOAT           NOT NULL,\n"
        "    gear            INT             NOT NULL,\n"
        "    trial_id        INT             NOT NULL,\n"
        "    tick            BIGINT UNSIGNED NOT NULL,\n"
        "    \n"
        "    CONSTRAINT game_state_id_primary_key PRIMARY KEY (game_state_id),\n"
        "    CONSTRAINT game_state_time_step_foreign_key FOREIGN KEY (trial_id, tick) REFERENCES TimeStep(trial_id, tick)\n"
        ")");

    EXECUTE(
        "CREATE TABLE IF NOT EXISTS UserInput (\n"
        "    user_input_id   BIGINT          NOT NULL AUTO_INCREMENT,\n"
        "    steer           FLOAT           NOT NULL,\n"
        "    brake           FLOAT           NOT NULL,\n"
        "    gas             FLOAT           NOT NULL,\n"
        "    clutch          FLOAT           NOT NULL,\n"
        "    trial_id        INT             NOT NULL,\n"
        "    tick            BIGINT UNSIGNED NOT NULL,\n"
        "    \n"
        "    CONSTRAINT user_input_id_primary_key PRIMARY KEY (user_input_id),\n"
        "    CONSTRAINT user_input_timestep_foreign_key FOREIGN KEY (trial_id, tick) REFERENCES TimeStep(trial_id, tick)\n"
        ")");

    EXECUTE(
        "CREATE TABLE IF NOT EXISTS Intervention (\n"
        "    intervention_id     BIGINT NOT NULL AUTO_INCREMENT,\n"
        "    trial_id            INT    NOT NULL,\n"
        "    tick                BIGINT UNSIGNED NOT NULL,\n"
        "    \n"
        "    CONSTRAINT decision_primary_key PRIMARY KEY (intervention_id),\n"
        "    CONSTRAINT intervention_timestep_foreign_key FOREIGN KEY (trial_id, tick) REFERENCES TimeStep(trial_id, tick)\n"
        ")");

    EXECUTE(
        "CREATE TABLE IF NOT EXISTS SteerDecision (\n"
        "    intervention_id     BIGINT  NOT NULL,\n"
        "    amount              FLOAT   NOT NULL,\n"
        "    \n"
        "    CONSTRAINT steer_id_primary_key PRIMARY KEY (intervention_id),\n"
        "    CONSTRAINT steer_key_is_intervention FOREIGN KEY (intervention_id) REFERENCES Intervention(intervention_id)\n"
        ")");

    EXECUTE(
        "CREATE TABLE IF NOT EXISTS BrakeDecision (\n"
        "    intervention_id     BIGINT  NOT NULL,\n"
        "    amount              FLOAT   NOT NULL,\n"
        "    \n"
        "    CONSTRAINT brake_id_primary_key PRIMARY KEY (intervention_id),\n"
        "    CONSTRAINT brake_key_is_intervention FOREIGN KEY (intervention_id) REFERENCES Intervention(intervention_id)\n"
        ")");

    EXECUTE(
        "CREATE TABLE IF NOT EXISTS LightsDecision (\n"
        "    intervention_id     BIGINT  NOT NULL,\n"
        "    turn_lights_on      BOOLEAN NOT NULL,\n"
        "    \n"
        "    CONSTRAINT lights_id_primary_key PRIMARY KEY (intervention_id),\n"
        "    CONSTRAINT lights_key_is_intervention FOREIGN KEY (intervention_id) REFERENCES Intervention(intervention_id)\n"
        ")");

    EXECUTE(
        "CREATE TABLE IF NOT EXISTS AccelDecision (\n"
        "    intervention_id     BIGINT  NOT NULL,\n"
        "    amount              FLOAT   NOT NULL,\n"
        "    \n"
        "    CONSTRAINT accel_id_primary_key PRIMARY KEY (intervention_id),\n"
        "    CONSTRAINT accel_key_is_intervention FOREIGN KEY (intervention_id) REFERENCES Intervention(intervention_id)\n"
        ")");

    EXECUTE(
        "CREATE TABLE IF NOT EXISTS GearDecision (\n"
        "    intervention_id     BIGINT  NOT NULL,\n"
        "    gear                INT     NOT NULL,\n"
        "    \n"
        "    CONSTRAINT gear_id_primary_key PRIMARY KEY (intervention_id),\n"
        "    CONSTRAINT gear_key_is_intervention FOREIGN KEY (intervention_id) REFERENCES Intervention(intervention_id)\n"
        ")");
}

/// @brief Creates a database and stores data from the buffer files into the correct database tables
/// @param p_bufferPaths Struct containg the paths to all the buffer files.
bool SQLDatabaseStorage::StoreData(const tBufferPaths& p_bufferPaths)
{
    std::ifstream metaDataStream(p_bufferPaths.MetaData);
    if (metaDataStream.fail())
    {
        std::cerr << "Buffer file not found: " << p_bufferPaths.MetaData.string() << std::endl;
        return false;
    }

    try
    {
        m_connection->setAutoCommit(false);
        int trialId = InsertMetaData(metaDataStream);
        SaveTrialIdToMetadata(trialId);
        InsertSimulationData(p_bufferPaths, trialId);
        m_connection->commit();
        return true;
    }
    catch (sql::SQLException& e)
    {
        LOG_MYSQL_EXCEPTION(e);
        m_connection->rollback();
        return false;
    }
    catch (std::exception& e)
    {
        m_connection->rollback();
        throw e;
    }
}

/// @brief Inserts meta data about the trial. Includes participant, blackbox, environment, settings, and trial.
/// @return trialId The auto-incremented id corresponding to this trial.
int SQLDatabaseStorage::InsertMetaData(std::ifstream& p_inputFileStream)
{
    sql::PreparedStatement* prepStmt;

    std::string values;

    // Store participant meta-data
    std::string participantId;

    READ_LINE(p_inputFileStream, participantId);
    EXECUTE(INSERT_IGNORE_INTO("Participant", "participant_id", participantId));

    // Store blackbox meta-data
    std::string blackboxFileName;
    std::string blackboxVersionDateTime;
    std::string blackboxName;

    READ_LINE(p_inputFileStream, blackboxFileName);
    READ_LINE(p_inputFileStream, blackboxVersionDateTime);
    READ_LINE(p_inputFileStream, blackboxName);

    values = "'" + blackboxFileName + "','" + blackboxVersionDateTime + "','" + blackboxName + "'";

    int blackboxId = -1;
    EXECUTE(INSERT_IGNORE_INTO("Blackbox", "filename, version, name", values));
    prepStmt = m_connection->prepareStatement(
        "SELECT blackbox_id FROM Blackbox WHERE filename = ? AND version = ?;");
    prepStmt->setString(1, blackboxFileName);
    prepStmt->setDateTime(2, blackboxVersionDateTime);
    GET_INT_FROM_PREP_QUERY(blackboxId, prepStmt);

    // Store environment meta-data
    std::string environmentFileName;
    std::string environmentVersion;
    std::string environmentName;

    READ_LINE(p_inputFileStream, environmentFileName);
    READ_LINE(p_inputFileStream, environmentVersion);
    READ_LINE(p_inputFileStream, environmentName);

    values = "'" + environmentFileName + "','" + environmentVersion + "','" + environmentName + "'";

    int environmentId = -1;
    EXECUTE(INSERT_IGNORE_INTO("Environment", "filename, version, name", values));
    prepStmt = m_connection->prepareStatement(
        "SELECT environment_id FROM Environment WHERE filename = ? AND version = ?;");
    prepStmt->setString(1, environmentFileName);
    prepStmt->setInt(2, std::stoi(environmentVersion));
    GET_INT_FROM_PREP_QUERY(environmentId, prepStmt);

    // Settings
    // Saved as enum index, but since indices in our code and MySQL are not the same, perform conversion
    std::string interventionMode;
    READ_LINE(p_inputFileStream, interventionMode);
    switch (std::stoi(interventionMode))
    {
        case INTERVENTION_TYPE_NO_SIGNALS:
            values = "'Off'";
            break;
        case INTERVENTION_TYPE_ONLY_SIGNALS:
            values = "'Suggest'";
            break;
        case INTERVENTION_TYPE_SHARED_CONTROL:
            values = "'Shared'";
            break;
        case INTERVENTION_TYPE_COMPLETE_TAKEOVER:
            values = "'Force'";
            break;
        case INTERVENTION_TYPE_AUTONOMOUS_AI:
            values = "'Drive'";
            break;
        default:
            std::cerr << "Invalid intervention type index read from buffer file" << std::endl;
            break;
    }

    int settingsId = -1;
    EXECUTE(INSERT_IGNORE_INTO("Settings", "intervention_mode", values));
    GET_INT_FROM_QUERY(settingsId,
                       "SELECT settings_id FROM Settings "
                       "WHERE intervention_mode = " + values);

    // Trial
    std::string trialDateTime;
    READ_LINE(p_inputFileStream, trialDateTime);
    values = "'" + trialDateTime + "','" + participantId + "','" + std::to_string(blackboxId) + "','" + std::to_string(environmentId) + "','" +
             std::to_string(settingsId) + "'";

    int trialId = -1;
    EXECUTE(INSERT_INTO("Trial", "trial_time, participant_id, blackbox_id, environment_id, settings_id", values));
    GET_INT_FROM_QUERY(trialId, "SELECT LAST_INSERT_ID()");

    return trialId;
}

/// @brief Inserts all simulation data into the database, by loading the corresponding buffer files.
///        Files are only loaded if this data needs to be stored.
/// @param p_bufferPaths Struct containg the paths to all the buffer files.
/// @param p_trialId     The id corresponding to this trial's data.
void SQLDatabaseStorage::InsertSimulationData(const tBufferPaths& p_bufferPaths, const int p_trialId)
{
    EXECUTE(
        "LOAD DATA LOCAL INFILE '" + AddEscapeCharacter(p_bufferPaths.TimeSteps) + "' INTO TABLE TimeStep "
        "   LINES TERMINATED BY '\\r\\n' IGNORE 1 LINES "
        "   (tick) "
        "   SET trial_id = " + std::to_string(p_trialId) + ";");

    if (m_dataToStore.CarData) InsertGameState(p_bufferPaths.GameState, p_trialId);
    if (m_dataToStore.HumanData) InsertUserInput(p_bufferPaths.UserInput, p_trialId);
    if (m_dataToStore.InterventionData) InsertDecisions(p_bufferPaths.Decisions, p_trialId);
}

/// @brief Loads the gamestate buffer file into the corresponding database table.
/// @param p_gameStatePath The buffer file to read from
/// @param p_trialId       The id corresponding to this trial's data.
void SQLDatabaseStorage::InsertGameState(const filesystem::path& p_gameStatePath, const int p_trialId)
{
    EXECUTE(
        "LOAD DATA LOCAL INFILE '" + AddEscapeCharacter(p_gameStatePath) + "' INTO TABLE GameState "
        "   FIELDS TERMINATED BY ',' LINES TERMINATED BY '\\r\n' IGNORE 1 LINES "
        "   (tick, x, y, z, direction_x, direction_y, direction_z, speed, acceleration, gear) "
        "   SET trial_id = " + std::to_string(p_trialId) + ";");
}

/// @brief Loads the userinput buffer file into the corresponding database table.
/// @param p_userInputPath The buffer file to read from
/// @param p_trialId       The id corresponding to this trial's data.
void SQLDatabaseStorage::InsertUserInput(const filesystem::path& p_userInputPath, const int p_trialId)
{
    EXECUTE(
        "LOAD DATA LOCAL INFILE '" + AddEscapeCharacter(p_userInputPath) + "' INTO TABLE UserInput "
        "   FIELDS TERMINATED BY ',' LINES TERMINATED BY '\\r\\n' IGNORE 1 LINES "
        "   (tick, steer, brake, gas, clutch) "
        "   SET trial_id = " + std::to_string(p_trialId) + ";");
}

/// @brief Loads the decisions buffer file into the database by first loading it into a temporary table.
///        From this table, the interventions are inserted and the auto-incremented id is stored into another temp table.
///        From this second temp table, all the decisions are then copied into the actual decision tables, linking to the intervention_id.
///        Decisions that are NULL (\N in the buffer file) are NOT stored.
/// @param p_decisionsPath The buffer file to read from
/// @param p_trialId       The id corresponding to this trial's data.
void SQLDatabaseStorage::InsertDecisions(const filesystem::path& p_decisionsPath, const int p_trialId)
{
    EXECUTE(
        "CREATE TEMPORARY TABLE TempInterventionData ( "
        "   temp_intervention_id BIGINT	         NOT NULL AUTO_INCREMENT, "
        "   temp_trial_id        INT             NOT NULL, "
        "   temp_tick            BIGINT UNSIGNED NOT NULL, "
        "   temp_steer_decision  FLOAT, "
        "   temp_brake_decision  FLOAT, "
        "   temp_accel_decision  FLOAT, "
        "   temp_gear_decision   INT, "
        "   temp_lights_decision BOOLEAN, "
        "   PRIMARY KEY(temp_intervention_id) "
        ");");

    // Load decisions csv file into temp table.
    EXECUTE(
        "LOAD DATA LOCAL INFILE '" + AddEscapeCharacter(p_decisionsPath) + "' INTO TABLE TempInterventionData "
        "   FIELDS TERMINATED BY ',' LINES TERMINATED BY '\\r\\n' IGNORE 1 LINES "
        "   (temp_tick, temp_steer_decision, temp_brake_decision, temp_accel_decision, temp_gear_decision, temp_lights_decision) "
        "   SET temp_trial_id = " + std::to_string(p_trialId) + ";");

    // Insert all data into the Intervention table
    EXECUTE(
        "INSERT INTO Intervention(trial_id, tick) "
        "   SELECT temp_trial_id, temp_tick "
        "   FROM TempInterventionData;");

    // Update intervention_ids in TempInterventionData with the AUTO_INCREMENTED intervention_ids from Intervention
    EXECUTE(
        "UPDATE TempInterventionData "
        "   JOIN Intervention ON TempInterventionData.temp_tick = Intervention.tick "
        "   AND TempInterventionData.temp_trial_id = Intervention.trial_id "
        "   SET TempInterventionData.temp_intervention_id = Intervention.intervention_id;");

    // Extract columns corresponding to each table from TempInterventionData
    EXECUTE(
        "INSERT INTO SteerDecision(intervention_id, amount) "
        "   SELECT temp_intervention_id, temp_steer_decision "
        "   FROM TempInterventionData "
        "   WHERE temp_steer_decision IS NOT NULL;");

    EXECUTE(
        "INSERT INTO BrakeDecision(intervention_id, amount) "
        "   SELECT temp_intervention_id, temp_brake_decision "
        "   FROM TempInterventionData "
        "   WHERE temp_brake_decision IS NOT NULL;");

    EXECUTE(
        "INSERT INTO AccelDecision(intervention_id, amount) "
        "   SELECT temp_intervention_id, temp_accel_decision "
        "   FROM TempInterventionData "
        "   WHERE temp_accel_decision IS NOT NULL;");

    EXECUTE(
        "INSERT INTO GearDecision(intervention_id, gear) "
        "   SELECT temp_intervention_id, temp_gear_decision "
        "   FROM TempInterventionData "
        "   WHERE temp_gear_decision IS NOT NULL;");

    EXECUTE(
        "INSERT INTO LightsDecision(intervention_id, turn_lights_on) "
        "   SELECT temp_intervention_id, temp_lights_decision "
        "   FROM TempInterventionData "
        "   WHERE temp_lights_decision IS NOT NULL;");
}

/// @brief Close the connection to the database and clean up.
///        sql::Statement and sql::Connection objects must be freed explicitly using delete
/// @param returnVal The value to return, indicating whether the database was succesfully used.
bool SQLDatabaseStorage::CloseDatabase(const bool p_returnVal)
{
    if (m_statement)
    {
        m_statement->close();
        delete m_statement;
    }

    if (m_connection)
    {
        m_connection->close();
        delete m_connection;
    }

    return p_returnVal;
}

/// @brief Runs the database storage with the given connection properties from the Mediator.
/// @param p_bufferPaths Struct containg the paths to all the buffer files.
bool SQLDatabaseStorage::Run(const tBufferPaths& p_bufferPaths)
{
    DatabaseSettings dbsettings = SMediator::GetInstance()->GetDatabaseSettings();

    bool openDb = OpenDatabase(dbsettings);
    if (!openDb) return CloseDatabase(false);
    std::cout << "Writing local buffer file to database" << std::endl;

    bool storeDb = StoreData(p_bufferPaths);
    if (!storeDb) return CloseDatabase(false);

    std::cout << "Finished writing to database" << std::endl;
    return CloseDatabase(true);
}

/// @brief Saves the trial id from the database to a buffer file with the meta data from a blackbox
/// @param p_trialId The trial id to save
void SQLDatabaseStorage::SaveTrialIdToMetadata(int p_trialId)
{
    filesystem::path bufferPath = filesystem::temp_directory_path();
    bufferPath.append(META_BUFFER_FILENAME);
    std::fstream blackboxBuffer;

    if (!filesystem::exists(bufferPath)) return;
    blackboxBuffer.open(bufferPath, std::ios::binary | std::fstream::in | std::fstream::out);

    blackboxBuffer.seekp(0, std::ios::beg);
    blackboxBuffer << bits(p_trialId);

    blackboxBuffer.flush();
    blackboxBuffer.close();
}
