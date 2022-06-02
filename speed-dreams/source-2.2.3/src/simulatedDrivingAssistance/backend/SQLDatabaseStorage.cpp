#include "SQLDatabaseStorage.h"
#include "Mediator.h"
#include <string>
#include "RppUtils.hpp"
#include "ConfigEnums.h"
#include <config.h>

/// @brief reads input from input file, unless EOF has been reached
#define READ_INPUT(p_inputFile, p_string)                       \
    if (p_inputFile.eof())                                      \
    {                                                           \
        p_inputFile.close();                                    \
        THROW_RPP_EXCEPTION("Reached end of file prematurely"); \
    }                                                           \
    std::getline(p_inputFile, p_string);

/// @brief executes sql statement
#define EXECUTE(p_sql) \
    m_statement->execute(p_sql);

/// @brief executes sql statement with return value
#define EXECUTE_QUERY(p_sql) \
    m_resultSet = m_statement->executeQuery(p_sql);

/// @brief create sql insert ignore statement
#define INSERT_IGNORE_INTO(p_tableName, p_headers, p_values) \
    "INSERT IGNORE INTO " p_tableName "(" p_headers ") VALUES (" + p_values + ");"

/// @brief create sql insert statement
#define INSERT_INTO(p_tableName, p_headers, p_values) \
    "INSERT INTO " p_tableName "(" p_headers ") VALUES (" + p_values + ");"

/// @brief executes sql statement to get latest inserted id in database with safety net
#define SELECT_LAST_ID(p_id)                                            \
    m_resultSet = m_statement->executeQuery("SELECT LAST_INSERT_ID()"); \
    while (m_resultSet->next()) { p_id = m_resultSet->getInt(1); };

/// @brief get int from result set
#define GET_INT_FROM_RESULTS(p_int) \
    while (m_resultSet->next()) p_int = m_resultSet->getInt(1);

/// @brief the amount of decision types a black box can create each tick
#define DECISIONS_AMOUNT 5

/// @brief The constructor of the SQL database storage
SQLDatabaseStorage::SQLDatabaseStorage()
{
    m_driver = nullptr;
    m_connection = nullptr;
    m_statement = nullptr;
    m_resultSet = nullptr;
};

/// @brief Creates a database and stores data from input file into the correct database structure
/// @param p_inputFilePath path and name of input file
bool SQLDatabaseStorage::StoreData(const filesystem::path& p_inputFilePath)
{
    // Check the existence of an input file
    std::ifstream inputFile(p_inputFilePath);
    if (inputFile.fail())
    {
        std::cerr << "[MYSQL] internal dberror: Could not open input file" << std::endl;
        return false;
    }

    try
    {
        int trial_id = InsertInitialData(inputFile);
        InsertSimulationData(inputFile, trial_id);
    }
    catch (std::exception& e)
    {
        CloseDatabase();
        inputFile.close();
        std::cerr << "[MYSQL] internal dberror: " << e.what() << std::endl;
        return false;
    }

    inputFile.close();

    return true;
}

/// @brief  gets the keys for secure database connection in the data/certificates folder
///         and adds them to the connection properties
///         name of the keys are set in the database_encryption_settings.txt file
/// @param p_connectionProperties   SQL connection properties to which the keys are added.
/// @param p_dbSettings             The database settings used to retrieve the certificate names.
void SQLDatabaseStorage::PutKeys(sql::ConnectOptionsMap& p_connectionProperties, DatabaseSettings p_dbSettings)
{
    p_connectionProperties["sslCA"] = p_dbSettings.CACertFilePath;
    p_connectionProperties["sslCert"] = p_dbSettings.PublicCertFilePath;
    p_connectionProperties["sslKey"] = p_dbSettings.PrivateCertFilePath;
}

/// @brief                  Connect to the specified database.
///                         Initialise the database with the proper tables if they don't exist yet.
/// @param p_dbSettings     Struct containing all settings for a database connection
/// @param p_dirPath        optional: path relative to the datafolder in which the
///                         "database_encryption_settings.txt" file is located
///                         needs "\\" in front
/// @return                 returns true if connection to database has been made, false otherwise
bool SQLDatabaseStorage::OpenDatabase(
    DatabaseSettings p_dbSettings,
    const std::string& p_dirPath)
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
    connection_properties["sslEnforce"] = true;

    if (p_dbSettings.UseSSL)
    {
        PutKeys(connection_properties, p_dbSettings);
    }

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
    EXECUTE("CREATE DATABASE IF NOT EXISTS " + schema)
    m_statement->close();
    delete m_statement;

    // Set the correct database schema
    m_connection->setSchema(p_dbSettings.Schema);

    // Create a (reusable) statement
    m_statement = m_connection->createStatement();

    // Our system will always use UTC times. Ensure the database knows this as well.
    EXECUTE("SET @@session.time_zone='+00:00';");

    // Ensure all tables are created
    CreateTables();

    return true;
}

/// @brief Executes sql create statements to create all tables
// TODO: update these to match the values of DataToStore in ConfigEnums
//  The current setup uses Gamestate (which includes both car and environment data) for "Environment data" checkbox,
//  UserInput for "Human data" and Decisions for "Intervention data",
//  but there should be separate tables for environment, car and internal metadata.
void SQLDatabaseStorage::CreateTables()
{
    EXECUTE(
        "CREATE TABLE IF NOT EXISTS Participant (\n"
        "    participant_id VARCHAR(255) NOT NULL,    \n"
        "    \n"
        "    CONSTRAINT participant_id_primary_key PRIMARY KEY (participant_id)\n"
        ")\n")

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
        ")\n")

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
        ")\n")

    EXECUTE(
        "CREATE TABLE IF NOT EXISTS Settings (\n"
        "    settings_id         INT  NOT NULL AUTO_INCREMENT,\n"
        "    intervention_mode   ENUM('Drive', 'Force', 'Shared', 'Suggest', 'Off') NOT NULL DEFAULT 'Off',\n"
        "    \n"
        "    CONSTRAINT settings_id_primary_key PRIMARY KEY (settings_id),\n"
        "    CONSTRAINT settings_unique UNIQUE (intervention_mode)\n"
        "    \n"
        ")")

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
        ")")

    EXECUTE(
        "CREATE TABLE IF NOT EXISTS TimeStep (\n"
        "    trial_id        INT             NOT NULL,\n"
        "    tick            BIGINT UNSIGNED NOT NULL,\n"
        "    \n"
        "    CONSTRAINT trial_tick_primary_key    PRIMARY KEY (trial_id, tick),\n"
        "    CONSTRAINT trial_id_foreign_key      FOREIGN KEY (trial_id) REFERENCES Trial(trial_id)\n"
        ")")

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
        ")")

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
        ")")

    EXECUTE(
        "CREATE TABLE IF NOT EXISTS Intervention (\n"
        "    intervention_id     BIGINT NOT NULL AUTO_INCREMENT,\n"
        "    trial_id            INT    NOT NULL,\n"
        "    tick                BIGINT UNSIGNED NOT NULL,\n"
        "    \n"
        "    CONSTRAINT decision_primary_key PRIMARY KEY (intervention_id),\n"
        "    CONSTRAINT intervention_timestep_foreign_key FOREIGN KEY (trial_id, tick) REFERENCES TimeStep(trial_id, tick)\n"
        ")")

    EXECUTE(
        "CREATE TABLE IF NOT EXISTS SteerDecision (\n"
        "    intervention_id     BIGINT  NOT NULL,\n"
        "    amount              FLOAT   NOT NULL,\n"
        "    \n"
        "    CONSTRAINT steer_id_primary_key PRIMARY KEY (intervention_id),\n"
        "    CONSTRAINT steer_key_is_intervention FOREIGN KEY (intervention_id) REFERENCES Intervention(intervention_id)\n"
        ")")

    EXECUTE(
        "CREATE TABLE IF NOT EXISTS BrakeDecision (\n"
        "    intervention_id     BIGINT  NOT NULL,\n"
        "    amount              FLOAT   NOT NULL,\n"
        "    \n"
        "    CONSTRAINT brake_id_primary_key PRIMARY KEY (intervention_id),\n"
        "    CONSTRAINT brake_key_is_intervention FOREIGN KEY (intervention_id) REFERENCES Intervention(intervention_id)\n"
        ")")

    EXECUTE(
        "CREATE TABLE IF NOT EXISTS LightsDecision (\n"
        "    intervention_id     BIGINT  NOT NULL,\n"
        "    turn_lights_on      BOOLEAN NOT NULL,\n"
        "    \n"
        "    CONSTRAINT lights_id_primary_key PRIMARY KEY (intervention_id),\n"
        "    CONSTRAINT lights_key_is_intervention FOREIGN KEY (intervention_id) REFERENCES Intervention(intervention_id)\n"
        ")")

    EXECUTE(
        "CREATE TABLE IF NOT EXISTS AccelDecision (\n"
        "    intervention_id     BIGINT  NOT NULL,\n"
        "    amount              FLOAT   NOT NULL,\n"
        "    \n"
        "    CONSTRAINT accel_id_primary_key PRIMARY KEY (intervention_id),\n"
        "    CONSTRAINT accel_key_is_intervention FOREIGN KEY (intervention_id) REFERENCES Intervention(intervention_id)\n"
        ")")

    EXECUTE(
        "CREATE TABLE IF NOT EXISTS GearDecision (\n"
        "    intervention_id     BIGINT  NOT NULL,\n"
        "    gear                INT     NOT NULL,\n"
        "    \n"
        "    CONSTRAINT gear_id_primary_key PRIMARY KEY (intervention_id),\n"
        "    CONSTRAINT gear_key_is_intervention FOREIGN KEY (intervention_id) REFERENCES Intervention(intervention_id)\n"
        ")")
}

/// @brief Inserts the data that stays the same during a trial. Includes participant, blackbox, environment, settings, and trial
/// @return trialId
int SQLDatabaseStorage::InsertInitialData(std::ifstream& p_inputFile)
{
    std::string values;

    // participant
    std::string participantId;

    READ_INPUT(p_inputFile, participantId);

    EXECUTE(INSERT_IGNORE_INTO("Participant", "participant_id", participantId))

    std::string trialDateTime;

    READ_INPUT(p_inputFile, trialDateTime);

    // blackbox
    std::string blackboxFileName;
    std::string blackboxVersionDateTime;
    std::string blackboxName;

    READ_INPUT(p_inputFile, blackboxFileName);
    READ_INPUT(p_inputFile, blackboxVersionDateTime);
    READ_INPUT(p_inputFile, blackboxName);

    values = "'" + blackboxFileName + "','" + blackboxVersionDateTime + "','" + blackboxName + "'";

    EXECUTE(INSERT_IGNORE_INTO("Blackbox", "filename, version, name", values))
    EXECUTE_QUERY("SELECT blackbox_id FROM Blackbox WHERE filename = '" + blackboxFileName + "' AND version = '" + blackboxVersionDateTime + "'")

    int blackboxId;
    GET_INT_FROM_RESULTS(blackboxId)

    // environment
    std::string environmentFileName;
    std::string environmentVersion;
    std::string environmentName;

    READ_INPUT(p_inputFile, environmentFileName);
    READ_INPUT(p_inputFile, environmentVersion);
    READ_INPUT(p_inputFile, environmentName);

    values = "'" + environmentFileName + "','" + environmentVersion + "','" + environmentName + "'";

    EXECUTE(INSERT_IGNORE_INTO("Environment", "filename, version, name", values))
    EXECUTE_QUERY("SELECT environment_id FROM Environment WHERE filename = '" + environmentFileName + "' AND version = '" + environmentVersion + "'")

    int environmentId;
    GET_INT_FROM_RESULTS(environmentId)

    // settings
    // Saved as enum index, but since indices in our code and MySQL are not the same, perform conversion
    std::string interventionMode;
    READ_INPUT(p_inputFile, interventionMode);
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

    EXECUTE(INSERT_IGNORE_INTO("Settings", "intervention_mode", values))
    EXECUTE_QUERY(
        "SELECT settings_id FROM Settings WHERE "
        "intervention_mode = " +
        values)

    int settingsId = -1;
    GET_INT_FROM_RESULTS(settingsId);

    values = "'" + trialDateTime + "','" + participantId + "','" + std::to_string(blackboxId) + "','" + std::to_string(environmentId) + "','" +
             std::to_string(settingsId) + "'";

    // trial
    EXECUTE(INSERT_INTO("Trial", "trial_time, participant_id, blackbox_id, environment_id, settings_id", values))
    EXECUTE_QUERY("SELECT LAST_INSERT_ID()");

    int trialId = -1;
    SELECT_LAST_ID(trialId);

    return trialId;
}

/// @brief Inserts data that is different for each tick in one trial, included tick, user input, gamestate, time step, interventions, and decisions
/// @param p_trialId id of trial that the simulation is linked with
void SQLDatabaseStorage::InsertSimulationData(std::ifstream& p_inputFile, const int p_trialId)
{
    std::string values;

    bool saveGameState = false;
    bool saveUserInput = false;
    bool hasReadTick = false;

    std::string dataToSave;
    READ_INPUT(p_inputFile, dataToSave)
    if (dataToSave == "GameState")
    {
        saveGameState = true;
        READ_INPUT(p_inputFile, dataToSave)
    }
    if (dataToSave == "UserInput")
    {
        saveUserInput = true;
    }
    else
        hasReadTick = true;

    // The values are read as a string from p_inputFile, and used as a string in the sql statements,
    // therefore they are not converted to the type they actually are.
    while (!p_inputFile.eof())
    {
        // tick
        std::string tick;
        // if decisions aren't saved, dataToSave has read the tick value in the previous loop
        if (hasReadTick)
        {
            hasReadTick = false;
            tick = dataToSave;
        }
        else
        {
            READ_INPUT(p_inputFile, tick)
        }

        if (tick == "END") break;

        values = "'" + std::to_string(p_trialId) + "','" + tick + "'";

        EXECUTE(INSERT_INTO("TimeStep", "trial_id, tick", values))

        // gamestate
        if (saveGameState) InsertGameState(p_inputFile, p_trialId, tick);

        // user input
        if (saveUserInput) InsertUserInput(p_inputFile, p_trialId, tick);

        READ_INPUT(p_inputFile, dataToSave)
        if (dataToSave == "Decisions")
        {
            InsertDecisions(p_inputFile, p_trialId, tick);
            dataToSave.empty();
        }
        else
            hasReadTick = true;
    }
}

void SQLDatabaseStorage::InsertGameState(std::ifstream& p_inputFile, const int p_trialId, const std::string& p_tick)
{
    std::string values;

    // gamestate
    std::string x;
    std::string y;
    std::string z;
    std::string directionX;
    std::string directionY;
    std::string directionZ;
    std::string speed;
    std::string acceleration;
    std::string gear;

    READ_INPUT(p_inputFile, x);
    READ_INPUT(p_inputFile, y);
    READ_INPUT(p_inputFile, z);
    READ_INPUT(p_inputFile, directionX);
    READ_INPUT(p_inputFile, directionY);
    READ_INPUT(p_inputFile, directionZ);
    READ_INPUT(p_inputFile, speed);
    READ_INPUT(p_inputFile, acceleration);
    READ_INPUT(p_inputFile, gear);

    values = "'";
    values.append(x);
    values.append("', '");
    values.append(y);
    values.append("', '");
    values.append(z);
    values.append("', '");
    values.append(directionX);
    values.append("', '");
    values.append(directionY);
    values.append("', '");
    values.append(directionZ);
    values.append("', '");
    values.append(speed);
    values.append("', '");
    values.append(acceleration);
    values.append("', '");
    values.append(gear);
    values.append("','");
    values.append(std::to_string(p_trialId));
    values.append("','");
    values.append(p_tick);
    values.append("'");

    EXECUTE(INSERT_INTO("GameState", "x, y, z, direction_x, direction_y, direction_z, speed, acceleration, gear, trial_id, tick", values));
}

void SQLDatabaseStorage::InsertUserInput(std::ifstream& p_inputFile, const int p_trialId, const std::string& p_tick)
{
    std::string values;

    std::string steer;
    std::string brake;
    std::string gas;
    std::string clutch;

    READ_INPUT(p_inputFile, steer);
    READ_INPUT(p_inputFile, brake);
    READ_INPUT(p_inputFile, gas);
    READ_INPUT(p_inputFile, clutch);

    values = "'" + steer + "','" + brake + "','" + gas + "','" + clutch + "','" + std::to_string(p_trialId) + "','" + p_tick + "'";
    EXECUTE(INSERT_INTO("UserInput", "steer, brake, gas, clutch, trial_id, tick", values));
}

/// @brief Loops through the input file and inserts all decisions that the black box has made
/// @param p_inputFile the input file to read from
/// @param p_trialId the trial the decisions are made in
/// @param p_tick the tick the decisions are made in
void SQLDatabaseStorage::InsertDecisions(std::ifstream& p_inputFile, const int p_trialId, const std::string& p_tick)
{
    std::string decision;
    READ_INPUT(p_inputFile, decision);
    // there shouldn't be more than DECISIONS_AMOUNT decisions made
    int decisionsRead = 0;
    while (decision != "NONE" && decisionsRead++ < DECISIONS_AMOUNT)
    {
        m_statement->execute(INSERT_INTO("Intervention", "trial_id, tick", ("'" + std::to_string(p_trialId) + "','" + p_tick + "'")));
        int decisionId;
        SELECT_LAST_ID(decisionId)

        if (decision == "SteerDecision")
        {
            std::string amount;
            READ_INPUT(p_inputFile, amount);
            EXECUTE(INSERT_INTO("SteerDecision", "intervention_id, amount", ("'" + std::to_string(decisionId) + "','" + amount + "'")));
        }
        else if (decision == "BrakeDecision")
        {
            std::string amount;
            READ_INPUT(p_inputFile, amount);
            EXECUTE(INSERT_INTO("BrakeDecision", "intervention_id, amount", ("'" + std::to_string(decisionId) + "','" + amount + "'")));
        }
        else if (decision == "AccelDecision")
        {
            std::string amount;
            READ_INPUT(p_inputFile, amount);
            EXECUTE(INSERT_INTO("AccelDecision", "intervention_id, amount", ("'" + std::to_string(decisionId) + "','" + amount + "'")));
        }
        else if (decision == "GearDecision")
        {
            std::string gear;
            READ_INPUT(p_inputFile, gear);
            EXECUTE(INSERT_INTO("GearDecision", "intervention_id, gear", ("'" + std::to_string(decisionId) + "','" + gear + "'")));
        }
        else if (decision == "LightsDecision")
        {
            std::string lightsOn;
            READ_INPUT(p_inputFile, lightsOn);
            EXECUTE(INSERT_INTO("LightsDecision", "intervention_id, turn_lights_on", ("'" + std::to_string(decisionId) + "','" + lightsOn + "'")));
        }

        READ_INPUT(p_inputFile, decision);
    }
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

/// @brief   Runs the database connection by finding the "database_connection_settings.txt" file
///          getting the connection properties out of that file
///          opening the database, storing the data of the bufferfile
///          and closing the database
/// @param   p_inputFilePath     path for the buffer file, the content of which is written to the database
/// @param   p_dirPath           optional parameter: path after the data folder
///                              for "database_connection_settings.txt" file
///                              and if applicable the "database_encryption_settings.txt".
///                              needs "\\" in front
///                              if left out path will be data folder
/// @returns Whether it has successfully written the local buffer file to the database
bool SQLDatabaseStorage::Run(const filesystem::path& p_inputFilePath, const std::string& p_dirPath)
{
    DatabaseSettings dbsettings = SMediator::GetInstance()->GetDatabaseSettings();

    bool open_success = OpenDatabase(dbsettings, p_dirPath);
    if (!open_success) return false;
    std::cout << "Writing local buffer file to database" << std::endl;

    bool store_success = StoreData(p_inputFilePath);
    if (!store_success)
    {
        CloseDatabase();
        return false;
    }
    CloseDatabase();
    std::cout << "Finished writing to database" << std::endl;
    return true;
}
