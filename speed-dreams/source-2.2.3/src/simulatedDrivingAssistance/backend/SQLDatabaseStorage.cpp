#include <iostream>
#include "SQLDatabaseStorage.h"
#include "sqlite3.h"
#include <string>
#include <sstream>
#include "../rppUtils/RppUtils.hpp"

#define DATABASE_THROW(db, error)                     \
    if (rc)                                           \
    {                                                 \
        std::string errorString(error);               \
        sqlite3_close(db);                            \
        remove(databasePath.c_str());\
        if (zErrMsg != nullptr)                       \
        {                                             \
            std::string s(zErrMsg);                   \
            errorString +=                            \
                "\n\tReturned with error: "           \
                + s;                                  \
        }                                             \
        errorString += "\n";                          \
        sqlite3_free(zErrMsg);                        \
        input.close();                                \
        throw std::runtime_error(errorString.c_str());\
    };

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

// Create a callback function
int callback(void *NotUsed, int argc, char **argv, char **azColName){


    // Return successful
    return 0;
}

void SQLDatabaseStorage::StoreData(const std::string p_filePath)
{
    std::string SimulationDataPath("source-2.2.3\\SimulationData\\");
    if (!FindFileDirectory(SimulationDataPath, p_filePath)) throw std::exception("Cannot find SimulationData directory");

    std::ifstream input(SimulationDataPath + p_filePath);

    if (!input.good()) throw std::exception("Cannot find/open input file");

    std::string reading;
    std::stringstream headers;
    std::stringstream sqlStream;

    input >> reading;

    const std::string tableName = "Simulation_" + reading;

    sqlStream << "CREATE TABLE " << tableName << " (TimeStamp int NOT NULL PRIMARY KEY, ";

    input >> reading;

    if (reading == "START") throw std::exception("Variable names were not given");

    // starts at one because of timestamp
    int variableAmount = 1;

    while (true)
    {
        variableAmount++;
        headers << reading;
        try
        {
            sqlStream << m_headerTypes.at(reading);
        }
        catch (const std::exception& e) {throw std::exception("Variable does not exist in map");}
        input >> reading;
        if (input.eof()) throw std::exception("Reached End Of File prematurely. Missing START?");
        if (reading == "START") break;
        headers << ", ";
        sqlStream << ", ";
    }

    sqlStream << ");";

    sqlite3* database;
    char* zErrMsg = nullptr;

    const std::string databaseFileName = tableName + "_database.sqlite3";
    const std::string databasePath =  SimulationDataPath + databaseFileName;

    std::ifstream databaseFile(databasePath);
    if (databaseFile.good()) throw std::runtime_error("A file with the name " + databaseFileName + " already exists");

    // open database
    int rc = sqlite3_open(databasePath.c_str(), &database);
    DATABASE_THROW(database, "Could not open database")

    // create table
    rc = sqlite3_exec(database,sqlStream.str().c_str(), callback, 0, &zErrMsg);
    DATABASE_THROW(database, "Could not create table.")

    sqlStream.clear();
    sqlStream.str("");

    while (true)
    {
        sqlStream
            << "INSERT INTO "
            << tableName
            << " (TimeStamp, "
            << headers.str()
            << ") VALUES (";

        for (int i = 0; i < variableAmount; i++)
        {
            input >> reading;
            sqlStream << "'" << reading << "'";
            if (i >= variableAmount - 1) break;
            sqlStream << ", ";
        }

        sqlStream << ");";

        if (input.eof()) break;
    }

    rc = sqlite3_exec(database, sqlStream.str().c_str(), callback, 0, &zErrMsg);
    DATABASE_THROW(database, "Could not insert items.")

    input.close();
    // avoid memory leak
    sqlite3_free(zErrMsg);

    sqlite3_close(database);
}