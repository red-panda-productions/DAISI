#include "SQLDatabaseStorage.h"
#include "Mediator.h"
#include <string>
#include <regex>
#include "RppUtils.hpp"
#include "ConfigEnums.h"
#include <config.h>

/// @brief reads input from input file, unless EOF has been reached
#define READ_LINE(p_inputFileStream, p_string)                   \
    if (p_inputFileStream.eof())                                 \
    {                                                            \
        p_inputFileStream.close();                               \
        THROW_RPP_EXCEPTION("Reached end of file prematurely");  \
    }                                                            \
    std::getline(p_inputFileStream, p_string);

/// @brief executes sql statement
#define EXECUTE(p_sql) \
    m_statement->execute(p_sql)

/// @brief create sql insert ignore statement
#define INSERT_IGNORE_INTO(p_tableName, p_headers, p_values) \
    "INSERT IGNORE INTO " p_tableName "(" p_headers ") VALUES (" + p_values + ");"

/// @brief create sql insert statement
#define INSERT_INTO(p_tableName, p_headers, p_values) \
    "INSERT INTO " p_tableName "(" p_headers ") VALUES (" + p_values + ");"

/// @brief Executes a query and retrieves the integer result
#define GET_INT_FROM_QUERY(p_int, p_querySql)            \
    m_resultSet = m_statement->executeQuery(p_querySql); \
    while (m_resultSet->next()) p_int = m_resultSet->getInt(1); 

#define STORE_ID_TRIGGER_NAME "store_id_trigger"



/// @brief The constructor of the SQL database storage
SQLDatabaseStorage::SQLDatabaseStorage()
    : SQLDatabaseStorage({true, true, true, true, true}) {};

/// @brief The constructor of the SQL database storage
SQLDatabaseStorage::SQLDatabaseStorage(tDataToStore p_dataToStore)
{
    m_driver = nullptr;
    m_connection = nullptr;
    m_statement = nullptr;
    m_resultSet = nullptr;
    m_dataToStore = p_dataToStore;
};

/// @brief Creates a database and stores data from input file into the correct database structure
/// @param p_inputFilePath path and name of input file
void SQLDatabaseStorage::StoreData(const tBufferPaths& p_bufferPaths)
{
    std::ifstream metaDataStream(p_bufferPaths.MetaData);
    if (metaDataStream.fail())
    {
        std::cerr << "[MYSQL] Buffer file not found: " << p_bufferPaths.MetaData.string() << std::endl;
        return;
    }

    try
    {
        m_connection->setAutoCommit(false);
        int trialId = InsertMetaData(metaDataStream);
        InsertSimulationData(p_bufferPaths, trialId);
        m_connection->commit();
    }
    catch (std::exception& e)
    {
        std::cerr << "[MYSQL] internal dberror: " << e.what() << std::endl;
    }
}

/// @brief  gets the keys for secure database connection in the data/certificates folder
///         and adds them to the connection properties
///         name of the keys are set in the database_encryption_settings.txt file
/// @param p_connectionProperties   SQL connection properties to which the keys are added.
/// @param p_dbSettings             The database settings used to retrieve the certificate names.
void SQLDatabaseStorage::PutKeys(sql::ConnectOptionsMap& p_connectionProperties, DatabaseSettings p_dbSettings)
{
    p_connectionProperties["sslVerify"] = p_dbSettings.VerifySSL;
    p_connectionProperties["sslCA"] = p_dbSettings.CACertFilePath;
    p_connectionProperties["sslCert"] = p_dbSettings.PublicCertFilePath;
    p_connectionProperties["sslKey"] = p_dbSettings.PrivateCertFilePath;
}

/// @brief                  Connect to the specified database.
///                         Initialise the database with the proper tables if they don't exist yet.
/// @param p_dbSettings     Struct containing all settings for a database connection
/// @return                 returns true if connection to database has been made, false otherwise
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
    catch (std::exception& e)
    {
        std::cerr << "Could not open database" << e.what() << std::endl;
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

/// @brief Executes sql create statements to create all tables
// TODO: update these to match the values of DataToStore in ConfigEnums
//  The current setup uses Gamestate for the CarData checkbox,
//  UserInput for "Human data" and Decisions for "Intervention data",
//  but there should be separate tables for environment, car and internal metadata.
void SQLDatabaseStorage::CreateTables()
{
    EXECUTE(
        "CREATE TABLE IF NOT EXISTS Participant (\n"
        "    participant_id VARCHAR(255) NOT NULL,    \n"
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

/// @brief Inserts the data that stays the same during a trial. Includes participant, blackbox, environment, settings, and trial
/// @return trialId
int SQLDatabaseStorage::InsertMetaData(std::ifstream& p_inputFileStream)
{
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
    GET_INT_FROM_QUERY(blackboxId,
        "SELECT blackbox_id FROM Blackbox "
        "WHERE filename = '" + blackboxFileName + "' AND version = '" + blackboxVersionDateTime + "'")

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
    GET_INT_FROM_QUERY(environmentId, 
        "SELECT environment_id FROM Environment "
        "WHERE filename = '" + environmentFileName + "' AND version = '" + environmentVersion + "'")

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
            THROW_RPP_EXCEPTION("Invalid intervention type index read from buffer file");
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

/// @brief Inserts data that is different for each tick in one trial, included tick, user input, gamestate, time step, interventions, and decisions
/// @param p_trialId id of trial that the simulation is linked with
void SQLDatabaseStorage::InsertSimulationData(const tBufferPaths& p_bufferPaths, const int p_trialId)
{
    EXECUTE(
        "LOAD DATA LOCAL INFILE '" + Escape(p_bufferPaths.TimeSteps) + "' INTO TABLE TimeStep "
        "   LINES TERMINATED BY '\\n' IGNORE 1 LINES "
        "   (tick) "
        "   SET trial_id = " + std::to_string(p_trialId) + ";");

    if (m_dataToStore.CarData) InsertGameState(p_bufferPaths.GameState, p_trialId);
    if (m_dataToStore.HumanData) InsertUserInput(p_bufferPaths.UserInput, p_trialId);
    if (m_dataToStore.InterventionData) InsertDecisions(p_bufferPaths.Decisions, p_trialId);
}

void SQLDatabaseStorage::InsertGameState(const filesystem::path& p_gameStatePath, const int p_trialId)
{
    EXECUTE(
        "LOAD DATA LOCAL INFILE '" + Escape(p_gameStatePath) + "' INTO TABLE GameState "
        "   FIELDS TERMINATED BY ',' LINES TERMINATED BY '\n' IGNORE 1 LINES "
        "   (tick, x, y, z, direction_x, direction_y, direction_z, speed, acceleration, gear) "
        "   SET trial_id = " + std::to_string(p_trialId) + ";");
}

void SQLDatabaseStorage::InsertUserInput(const filesystem::path& p_userInputPath, const int p_trialId)
{
    EXECUTE(
        "LOAD DATA LOCAL INFILE '" + Escape(p_userInputPath) + "' INTO TABLE UserInput "
        "   FIELDS TERMINATED BY ',' LINES TERMINATED BY '\\n' IGNORE 1 LINES "
        "   (tick, steer, brake, gas, clutch) "
        "   SET trial_id = " + std::to_string(p_trialId) + ";");
}

/// @brief Loops through the input file and inserts all decisions that the black box has made
/// @param p_inputFileStream the input file to read from
/// @param p_trialId the trial the decisions are made in
/// @param p_tick the tick the decisions are made in
void SQLDatabaseStorage::InsertDecisions(const filesystem::path& p_decisionsPath, const int p_trialId)
{
    EXECUTE(
        "CREATE TEMPORARY TABLE TempInterventionData ( "
        "   temp_intervention_id BIGINT			 NOT NULL AUTO_INCREMENT, "
        "   temp_trial_id        INT             NOT NULL, "
        "   temp_tick            BIGINT UNSIGNED NOT NULL, "
        "   temp_steer_decision  FLOAT           NOT NULL, "
        "   temp_brake_decision  FLOAT           NOT NULL, "
        "   temp_accel_decision  FLOAT           NOT NULL, "
        "   temp_gear_decision   INT             NOT NULL, "
        "   temp_lights_decision BOOLEAN         NOT NULL, "
        "   PRIMARY KEY(temp_intervention_id) "
        ");");

    // Load decisions csv file into temp table.
    EXECUTE(
        "LOAD DATA LOCAL INFILE '" + Escape(p_decisionsPath) + "' INTO TABLE TempInterventionData "
        "   FIELDS TERMINATED BY ',' LINES TERMINATED BY '\\n' IGNORE 1 LINES "
        "   (temp_tick, temp_steer_decision, temp_brake_decision, temp_accel_decision, temp_gear_decision, temp_lights_decision) "
        "   SET temp_trial_id = " + std::to_string(p_trialId) + ";");

    // Create a copy of the temp table, which will be used to insert the auto_incremented intervention_id into.
    EXECUTE("CREATE TEMPORARY TABLE TempInterventionDataWithCorrectId AS SELECT * FROM TempInterventionData;");

    // Create trigger for saving the auto incremented id into (a copy of) the temp table.
    EXECUTE(
        "CREATE TRIGGER " STORE_ID_TRIGGER_NAME " AFTER INSERT ON Intervention"
        "   FOR EACH ROW"
        "       UPDATE TempInterventionDataWithCorrectId"
        "       SET temp_intervention_id = NEW.intervention_id"
        "       WHERE temp_tick = NEW.tick;");

    // Insert all interventions, which will trigger the auto_incremented id to be stored in TempInterventionDataWithCorrectId.
    EXECUTE(
        "INSERT INTO Intervention(trial_id, tick) "
        "   SELECT temp_trial_id, temp_tick "
        "   FROM TempInterventionData;");

    // Extract columns corresponding to each table from TempInterventionDataWithCorrectId
    EXECUTE(
        "INSERT INTO SteerDecision(intervention_id, amount) "
        "   SELECT temp_intervention_id, temp_steer_decision "
        "   FROM TempInterventionDataWithCorrectId;");

    EXECUTE(
        "INSERT INTO BrakeDecision(intervention_id, amount) "
        "   SELECT temp_intervention_id, temp_brake_decision "
        "   FROM TempInterventionDataWithCorrectId;");

    EXECUTE(
        "INSERT INTO AccelDecision(intervention_id, amount) "
        "   SELECT temp_intervention_id, temp_accel_decision "
        "   FROM TempInterventionDataWithCorrectId;");

    EXECUTE(
        "INSERT INTO GearDecision(intervention_id, gear) "
        "   SELECT temp_intervention_id, temp_gear_decision "
        "   FROM TempInterventionDataWithCorrectId;");

    EXECUTE(
        "INSERT INTO LightsDecision(intervention_id, turn_lights_on) "
        "   SELECT temp_intervention_id, temp_lights_decision "
        "   FROM TempInterventionDataWithCorrectId;");

    EXECUTE("DROP TRIGGER " STORE_ID_TRIGGER_NAME ";");
}

/// @brief Close the connection to the database and clean up.
///        sql::Statement and sql::Connection objects must be freed explicitly using delete
///        Might be called in the catch of an error already, so guard when the variables are already deleted.
void SQLDatabaseStorage::CloseDatabase()
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
}

/// @brief  Runs the database connection by finding the "database_connection_settings.txt" file
///         getting the connection properties out of that file
///         opening the database, storing the data of the bufferfile
///         and closing the database
/// @param  p_inputFilePath     path for the buffer file, the content of which is written to the database
/// @param  p_dirPath           optional parameter: path after the data folder
///                             for "database_connection_settings.txt" file
///                             and if applicable the "database_encryption_settings.txt".
///                             needs "\\" in front
///                             if left out path will be data folder
void SQLDatabaseStorage::Run(const tBufferPaths& p_bufferPaths)
{
    DatabaseSettings dbsettings = SMediator::GetInstance()->GetDatabaseSettings();

    if (OpenDatabase(dbsettings))
    {
        std::cout << "Writing local buffer file to database" << std::endl;
        StoreData(p_bufferPaths);
        CloseDatabase();
        std::cout << "Finished writing to database" << std::endl;
    }
}
