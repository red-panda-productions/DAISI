#include "SQLDatabaseStorage.h"
#include <string>
#include "mysqlx/xdevapi.h"
#include "../rppUtils/RppUtils.hpp"

#define INSERT_INTO(tableName, headers, values) \
  "INSERT IGNORE INTO" tableName "(" headers ") VALUES (" + values + ");"

/// @brief The constructor of the SQL database storage
SQLDatabaseStorage::SQLDatabaseStorage() {
    m_driver = nullptr;
    m_connection = nullptr;
    m_statement = nullptr;
    m_resultSet = nullptr;
};

/// @brief Creates a database and stores data from input file into a table
/// @param p_inputFilePath path and name of input file (from SimulationData\)
void SQLDatabaseStorage::StoreData(const std::string p_inputFilePath)
{
    // Check the existence of an input file
    m_inputFile.open(p_inputFilePath);
    if (!m_inputFile.good()) throw std::exception("Could not open data file");

    bool newDatabase = false;
    OpenDatabase("localhost", 3306, "root", "root", "test", newDatabase);

    InsertInitialData();
    InsertSimulationData();

    m_inputFile.close();
    CloseDatabase();
}

/// @brief Connect to the specified database. If the database is new, initialise the database with the proper table.
/// @param p_hostName Hostname of the database to connect to. Should be a TCP-IP address.
/// @param p_port Port the database is located on on the host.
/// @param p_username Username to connect with to the database.
/// @param p_password Password to connect with to the database.
/// @param p_schemaName Name of the database schema to use. Schema should exist already, but does not need to contain any tables.
/// @param p_isNewSchema Whether the database schema is new. If new, the schema will be initialised with the proper tables.
void SQLDatabaseStorage::OpenDatabase(
    const std::string& p_hostName,
    int p_port,
    const std::string& p_username,
    const std::string& p_password,
    const std::string& p_schemaName,
    bool p_isNewSchema)
{
    // Initialise SQL driver
    m_driver = sql::mysql::get_mysql_driver_instance();

    // Set connection options, and connect to the database
    sql::ConnectOptionsMap connection_properties;
    connection_properties["hostName"] = "tcp://" + p_hostName;
    connection_properties["userName"] = p_username;
    connection_properties["password"] = p_password;
    connection_properties["port"] = p_port;
    connection_properties["OPT_RECONNECT"] = true;
    connection_properties["CLIENT_MULTI_STATEMENTS"] = true;
    m_connection = m_driver->connect(connection_properties);

    // Create the database schema if this is a new schema. This has to be done before setting the schema on the connection.
    if (p_isNewSchema) {
        m_statement = m_connection->createStatement();
        m_statement->execute("CREATE DATABASE IF NOT EXISTS " + p_schemaName);
        m_statement->close();
        delete m_statement;
    }

    // Set the correct database schema
    m_connection->setSchema(p_schemaName);

    // Create a (reusable) statement
    m_statement = m_connection->createStatement();
    // If the database schema is new, initialise it with tables
    if (p_isNewSchema) {
        CreateTables();
    }
}

void SQLDatabaseStorage::CreateTables()
{
    m_statement->execute(
            "CREATE TABLE Participant (\n"
            "    participant_id INT NOT NULL,    \n"
            "    \n"
            "    CONSTRAINT participant_id_primary_key PRIMARY KEY (participant_id)\n"
            ");\n"
            "\n"
            "CREATE TABLE Blackbox (\n"
            "    blackbox_id INT NOT NULL AUTO_INCREMENT,\n"
            "    filename    VARCHAR(255)    NOT NULL,\n"
            "    version     DATETIME(0)     NOT NULL,\n"
            "    name        VARCHAR(255)    NOT NULL,\n"
            "    \n"
            "    CONSTRAINT blackbox_id_primary_key PRIMARY KEY (blackbox_id),\n"
            "    \n"
            "    CONSTRAINT file_version UNIQUE (filename, version)\n"
            ");\n"
            "\n"
            "CREATE TABLE Environment (\n"
            "    environment_id  INT             NOT NULL AUTO_INCREMENT,\n"
            "    filename        VARCHAR(255)    NOT NULL,\n"
            "    version         DATETIME(0)     NOT NULL,\n"
            "    name            VARCHAR(255)    NOT NULL,\n"
            "    \n"
            "    CONSTRAINT environment_id_primary_key PRIMARY KEY (environment_id),\n"
            "    \n"
            "    CONSTRAINT file_version UNIQUE (filename, version)\n"
            ");\n"
            "\n"
            "CREATE TABLE Settings (\n"
            "    settings_id         INT  NOT NULL AUTO_INCREMENT,\n"
            "    intervention_mode   ENUM('Force', 'Ask', 'Suggest', 'Off') NOT NULL DEFAULT 'Off',\n"
            "    \n"
            "    CONSTRAINT settings_id_primary_key PRIMARY KEY (settings_id)\n"
            "    \n"
            ");\n"
            "\n"
            "CREATE TABLE Trial (\n"
            "    trial_id        INT         NOT NULL AUTO_INCREMENT,\n"
            "    trial_time      DATETIME(0) NOT NULL,\n"
            "    participant_id  INT         NOT NULL,\n"
            "    blackbox_id     INT         NOT NULL,\n"
            "    environment_id  INT         NOT NULL,\n"
            "    settings_id     INT         NOT NULL,\n"
            "    \n"
            "    CONSTRAINT trial_id_primary_key         PRIMARY KEY (trial_id),\n"
            "    CONSTRAINT participant_id_foreign_key   FOREIGN KEY (participant_id) REFERENCES Participant(participant_id),\n"
            "    CONSTRAINT blackbox_id_foreign_key      FOREIGN KEY (blackbox_id)    REFERENCES Blackbox(blackbox_id),\n"
            "    CONSTRAINT environment_id_foreign_key   FOREIGN KEY (environment_id) REFERENCES Environment(environment_id),\n"
            "    CONSTRAINT settings_id_foreign_key      FOREIGN KEY (settings_id)    REFERENCES Settings(settings_id)\n"
            ");\n"
            "\n"
            "CREATE TABLE UserInput (\n"
            "    user_input_id   INT     NOT NULL AUTO_INCREMENT,\n"
            "    steer           FLOAT   NOT NULL,\n"
            "    brake           FLOAT   NOT NULL,\n"
            "    gas             FLOAT   NOT NULL,\n"
            "    clutch          FLOAT   NOT NULL,\n"
            "    \n"
            "    CONSTRAINT user_input_id_primary_key PRIMARY KEY (user_input_id)\n"
            ");\n"
            "\n"
            "CREATE TABLE GameState (\n"
            "    game_state_id   INT     NOT NULL AUTO_INCREMENT,\n"
            "    x               DOUBLE  NOT NULL,\n"
            "    y               DOUBLE  NOT NULL,\n"
            "    z               DOUBLE  NOT NULL,\n"
            "    direction_x     DOUBLE  NOT NULL,\n"
            "    direction_y     DOUBLE  NOT NULL,\n"
            "    direction_z     DOUBLE  NOT NULL,\n"
            "    speed           FLOAT   NOT NULL,\n"
            "    acceleration    FLOAT   NOT NULL,\n"
            "    gear            INT     NOT NULL,\n"
            "    user_input_id   INT     NOT NULL,\n"
            "    \n"
            "    CONSTRAINT game_state_id_primary_key PRIMARY KEY (game_state_id),\n"
            "    CONSTRAINT user_input_id_foreign_key FOREIGN KEY (user_input_id) REFERENCES UserInput(user_input_id)\n"
            ");\n"
            "    \n"
            "CREATE TABLE TimeStep (\n"
            "    trial_id        INT             NOT NULL AUTO_INCREMENT,\n"
            "    tick            BIGINT UNSIGNED NOT NULL,\n"
            "    game_state_id   INT             NOT NULL,\n"
            "    \n"
            "    CONSTRAINT trial_tick_primary_key       PRIMARY KEY (trial_id, tick),\n"
            "    CONSTRAINT game_state_id_foreign_key    FOREIGN KEY (game_state_id)     REFERENCES GameState(game_state_id)\n"
            ");\n"
            "\n"
            "CREATE TABLE Intervention (\n"
            "    intervention_id     INT NOT NULL AUTO_INCREMENT,\n"
            "    trial_id            INT NOT NULL,\n"
            "    tick                BIGINT UNSIGNED NOT NULL,\n"
            "    \n"
            "    CONSTRAINT decision_primary_key PRIMARY KEY (intervention_id),\n"
            "    CONSTRAINT trial_tick_foreign_key FOREIGN KEY (trial_id, tick) REFERENCES TimeStep(trial_id, tick)\n"
            ");\n"
            "\n"
            "CREATE TABLE SteerDecision (\n"
            "    intervention_id     INT     NOT NULL,\n"
            "    amount              FLOAT   NOT NULL,\n"
            "    \n"
            "    CONSTRAINT steer_id_primary_key PRIMARY KEY (intervention_id),\n"
            "    CONSTRAINT steer_key_is_intervention FOREIGN KEY (intervention_id) REFERENCES Intervention(intervention_id)\n"
            ");\n"
            "\n"
            "CREATE TABLE BrakeDecision (\n"
            "    intervention_id     INT     NOT NULL,\n"
            "    amount              FLOAT   NOT NULL,\n"
            "    \n"
            "    CONSTRAINT brake_id_primary_key PRIMARY KEY (intervention_id),\n"
            "    CONSTRAINT brake_key_is_intervention FOREIGN KEY (intervention_id) REFERENCES Intervention(intervention_id)\n"
            ");\n"
            "\n"
            "CREATE TABLE LightsDecision (\n"
            "    intervention_id     INT     NOT NULL,\n"
            "    turn_lights_on      BOOLEAN NOT NULL,\n"
            "    \n"
            "    CONSTRAINT lights_id_primary_key PRIMARY KEY (intervention_id),\n"
            "    CONSTRAINT lights_key_is_intervention FOREIGN KEY (intervention_id) REFERENCES Intervention(intervention_id)\n"
            ");");
}

void SQLDatabaseStorage::InsertInitialData()
{
    std::string blackboxFileName;
    std::string blackboxVersion;
    std::string blackboxName;

    m_sstream << blackboxFileName;
    m_sstream << blackboxVersion;
    m_sstream << blackboxName;

    m_reading = "'" + blackboxFileName + "'," + blackboxVersion + "'," + blackboxName + "'";

    m_statement->execute(INSERT_INTO("Blackbox", "filename, version, name", m_reading));
    m_resultSet = m_statement->executeQuery(
            "SELECT (blackbox_id) FROM Blackbox \n"
            "    WHERE filename = " + blackboxFileName +
            "    AND version = " + blackboxVersion);

    int blackbox_id = m_resultSet->getInt(0);
}

void SQLDatabaseStorage::InsertSimulationData()
{

}

/// @brief Close the connection to the database and clean up.
void SQLDatabaseStorage::CloseDatabase() {
    m_statement->close();
    m_connection->close();

    // sql::Statement and sql::Connection objects must be freed explicitly using delete
    delete m_statement;
    delete m_connection;
}
