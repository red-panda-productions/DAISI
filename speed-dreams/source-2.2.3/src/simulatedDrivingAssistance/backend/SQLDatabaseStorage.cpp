#include <iostream>
#include "SQLDatabaseStorage.h"
#include "sqlite3.h"
#include <string>
#include <sstream>
#include "../rppUtils/RppUtils.hpp"

#define DATABASE_THROW(error) CheckForDatabaseError(rc, database, (std::string &) error, databasePath, zErrMsg, input);

SQLDatabaseStorage::SQLDatabaseStorage()
{
    // player info data
    m_headerTypes["SteerCmd"] = "SteerCmd float NOT NULL";
    m_headerTypes["AccelCmd"] = "AccelCmd float NOT NULL";
    m_headerTypes["BrakeCmd"] = "BrakeCmd float NOT NULL";
    m_headerTypes["ClutchCmd"] = "ClutchCmd float NOT NULL";

    // environment data
    m_headerTypes["TimeOfDay"] = "TimeOfDay int NOT NULL";
    m_headerTypes["Clouds"] = "Clouds int NOT NULL";
    m_headerTypes["Rain"] = "Rain int NOT NULL";

    // car data
    m_headerTypes["Speed"] = "Speed float NOT NULL";
    m_headerTypes["TopSpeed"] = "TopSpeed float NOT NULL";
    m_headerTypes["Gear"] = "Gear int NOT NULL";
    m_headerTypes["Headlights"] = "Headlights bool NOT NULL";

    // track position data
    m_headerTypes["Offroad"] = "Offroad bool NOT NULL";
    m_headerTypes["ToStart"] = "ToStart float NOT NULL";
    m_headerTypes["ToRight"] = "ToRight float NOT NULL";
    m_headerTypes["ToMiddle"] = "ToMiddle float NOT NULL";
    m_headerTypes["ToLeft"] = "ToLeft float NOT NULL";

    // AI decision data
    m_headerTypes["SteerDecision"] = "SteerDecision float NOT NULL";
    m_headerTypes["BrakeDecision"] = "BrakeDecision float NOT NULL";
}

void CheckForDatabaseError(int p_sqliteExec, sqlite3*& p_db, std::string& p_error, const std::string& p_databasePath, char* p_zErrMsg, std::ifstream& p_inputFile)
{
    if (p_sqliteExec)
    {
        sqlite3_close(p_db);
        remove(p_databasePath.c_str());
        if (p_zErrMsg != nullptr)
        {
            std::string s(p_zErrMsg);
            p_error +=
                "\n\tReturned with error: "
                + s;
        }
        p_error += "\n";
        sqlite3_free(p_zErrMsg);
        p_inputFile.close();
        throw std::exception(p_error.c_str());
    }
}

/// @brief Creates SQL query that will create a database table
/// @param p_inputFile data file to read from
/// @param p_reading current data read from file
/// @param p_tableName name of table to create
/// @param p_sqlStream string stream that collects all sql statements
/// @param p_headerTypes map that sets the correct type for each header name
/// @param p_variableAmount how many variables the table has
/// @param p_headers header names
void CreateTableSQL(
        std::ifstream& p_inputFile,
        std::string& p_reading,
        const std::string& p_tableName,
        std::stringstream& p_sqlStream,
        const std::unordered_map<std::string,std::string>& p_headerTypes,
        int& p_variableAmount,
        std::string& p_headers)
{
    std::stringstream headersStream;

    p_sqlStream.clear();
    p_sqlStream.str("");

    p_sqlStream << "CREATE TABLE " << p_tableName << " (TimeStamp int NOT NULL PRIMARY KEY, ";

    p_inputFile >> p_reading;

    if (p_reading == "START") throw std::exception("Variable names were not given");

    // starts at one because of timestamp
    int variableAmount = 1;

    while (true)
    {
        variableAmount++;
        headersStream << p_reading;
        try
        {
            p_sqlStream << p_headerTypes.at(p_reading);
        }
        catch (const std::exception& e) {throw std::exception("Variable does not exist in map");}
        p_inputFile >> p_reading;
        if (p_inputFile.eof()) throw std::exception("Reached End Of File prematurely. Missing START?");
        if (p_reading == "START") break;
        headersStream << ", ";
        p_sqlStream << ", ";
    }

    p_sqlStream << ");";

    p_variableAmount = variableAmount;
    p_headers = headersStream.str();
}

/// @brief Puts SQL Insert statements in sqlStream.
/// @params Same as CreateTableSQL
void InsertDataSQL(
        std::ifstream& p_inputFile,
        std::string& p_reading,
        const std::string& p_tableName,
        std::stringstream& p_sqlStream,
        const int p_variableAmount,
        const std::string& p_headers)
{
    p_sqlStream.clear();
    p_sqlStream.str("");

    while (true)
    {
        p_sqlStream
                << "INSERT INTO "
                << p_tableName
                << " (TimeStamp, "
                << p_headers
                << ") VALUES (";

        for (int i = 0; i < p_variableAmount; i++)
        {
            p_inputFile >> p_reading;
            p_sqlStream << "'" << p_reading << "'";
            if (i >= p_variableAmount - 1) break;
            p_sqlStream << ", ";
        }

        p_sqlStream << ");";

        if (p_inputFile.eof()) break;
    }
}

/// @brief Creates and open a database if it doesn't exist already
/// @param database pointer where the database will be stored
/// @param p_databasePath path to the database
/// @param p_simulationDataPath path to the simulation data folder
/// @param p_tableName name of the table that will be created
/// @return integer with information on successfulness of opening the database
int OpenDatabase(sqlite3*& database, std::string& p_databasePath, const std::string& p_simulationDataPath, const std::string& p_tableName)
{
    const std::string databaseFileName = p_tableName + "_database.sqlite3";
    p_databasePath =  p_simulationDataPath + databaseFileName;

    std::ifstream databaseFile(p_databasePath);
    if (databaseFile.good()) throw std::runtime_error("A file with the name " + databaseFileName + " already exists");

    // open database
    return sqlite3_open(p_databasePath.c_str(), &database);
}

/// @brief Creates a database and stores data from input file into a table
/// @param p_filePath path and name of input file (from SimulationData\)
void SQLDatabaseStorage::StoreData(const std::string p_filePath)
{
    std::string SimulationDataPath("SimulationData\\");
    if (!FindFileDirectory(SimulationDataPath, p_filePath)) throw std::exception("Cannot find SimulationData directory");

    std::ifstream input(SimulationDataPath + p_filePath);

    if (!input.good()) throw std::exception("Cannot find/open input file");

    std::string reading;
    std::stringstream sqlStream;

    // read first string: ID of participant
    input >> reading;
    const std::string tableName = "Simulation_" + reading;

    sqlite3* database;
    char* zErrMsg = nullptr;

    std::string databasePath;

    //int p_sqliteExec, sqlite3*& p_db, std::string& p_error, const std::string& p_databasePath, char* p_zErrMsg, std::ifstream& p_inputFile

    // open database
    int rc = OpenDatabase(database, databasePath, SimulationDataPath, tableName);
    DATABASE_THROW("Could not open database")

    int variableAmount;
    std::string headers;

    // create table in database
    CreateTableSQL(input,reading,tableName,sqlStream,m_headerTypes, variableAmount, headers);
    rc = sqlite3_exec(database, sqlStream.str().c_str(), nullptr, nullptr, &zErrMsg);
    DATABASE_THROW("Could not create table.")

    // insert data in table
    InsertDataSQL(input, reading, tableName, sqlStream, variableAmount, headers);
    rc = sqlite3_exec(database, sqlStream.str().c_str(), nullptr, nullptr, &zErrMsg);
    DATABASE_THROW("Could not insert items.")

    input.close();

    // avoid memory leak
    sqlite3_free(zErrMsg);

    sqlite3_close(database);
}