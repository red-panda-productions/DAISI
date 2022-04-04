#include "SQLDatabaseStorage.h"
#include "sqlite3.h"
#include <string>
#include "mysql/jdbc.h"
#include "../rppUtils/RppUtils.hpp"


/// @brief The constructor of the SQL database storage
SQLDatabaseStorage::SQLDatabaseStorage(std::string p_inputFilePath)
{
    m_inputFile.open(p_inputFilePath);
    if (!m_inputFile.good()) throw std::exception("Could not open data file");
}

/// @brief Creates a database and stores data from input file into a table
/// @param p_filePath path and name of input file (from SimulationData\)
void SQLDatabaseStorage::StoreData(const std::string p_filePath)
{
    bool newDatabase = false;
    OpenDatabase(p_filePath, newDatabase);
    if (newDatabase) CreateTables();

    InsertInitialData();
    InsertSimulationData();

    m_inputFile.close();
}

void SQLDatabaseStorage::OpenDatabase(std::string p_filePath, bool &p_newDatabase)
{
//    std::ifstream checkForDatabase(p_filePath);
//    if (!checkForDatabase.good()) p_newDatabase = true;
//    else checkForDatabase.close();

    m_driver = sql::mysql::get_mysql_driver_instance(); 
    sql::mysql::MySQL_Connection* connection;

    m_connection = m_driver->connect("tcp://127.0.0.1", "root", "root");
    m_connection->setSchema("test");
}

void SQLDatabaseStorage::CreateTables()
{

}

void SQLDatabaseStorage::InsertInitialData()
{

}

void SQLDatabaseStorage::InsertSimulationData()
{

}