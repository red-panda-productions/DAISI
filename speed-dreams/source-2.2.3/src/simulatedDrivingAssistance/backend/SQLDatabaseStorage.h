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
    SQLDatabaseStorage();
    void StoreData(const std::string p_inputFilePath) override;
    void OpenDatabase(const std::string& p_hostName,
                      int p_port,
                      const std::string& p_username,
                      const std::string& p_password,
                      const std::string& p_schemaName);

    void CloseDatabase();

private:
    void CreateTables();
    int InsertInitialData();
    void InsertSimulationData(const int p_trialId);

    std::string m_values;
    std::ifstream m_inputFile;
    sql::Driver* m_driver;
    sql::Connection* m_connection;
    sql::Statement* m_statement;
    sql::ResultSet* m_resultSet;
};