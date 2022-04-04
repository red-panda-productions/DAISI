#pragma once
#include "IDataStorage.h"
#include <string>
#include <unordered_map>
#include <sstream>
#include <fstream>
#include "mysql/jdbc.h"

/// @brief A class that can store data to a SQL database
class SQLDatabaseStorage : IDataStorage
{
public:
    SQLDatabaseStorage(std::string p_infputFilePath);
    void StoreData(const std::string p_filePath) override;

private:
    void OpenDatabase(std::string p_filePath, bool& p_newDatabase);
    void CreateTables();
    void InsertInitialData();
    void InsertSimulationData();

    std::string m_reading;
    std::ifstream m_inputFile;
    std::stringstream m_sstream;
    sql::mysql::MySQL_Driver* m_driver;
    sql::Connection* m_connection;
};