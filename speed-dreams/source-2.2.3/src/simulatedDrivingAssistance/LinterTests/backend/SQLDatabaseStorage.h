#pragma once
#include "IDataStorage.h"
#include <string>
#include <unordered_map>
#include <fstream>
#include "mysql/jdbc.h"

/// @brief A class that can store data to a SQL database
class SQLDatabaseStorage : IDataStorage
{
public:
    SQLDatabaseStorage();
    void Run(const std::string p_inputFilePath);
    void StoreData(const std::string p_inputFilePath) override;
    bool OpenDatabase(const std::string& p_hostName,
                      int p_port,
                      const std::string& p_username,
                      const std::string& p_password,
                      const std::string& p_schemaName);

    void CloseDatabase();

private:
    void CreateTables();
    int InsertInitialData(std::ifstream& p_inputFile);
    void InsertSimulationData(std::ifstream& p_inputFile, const int p_trialId);
    void InsertDecisions(std::ifstream& p_inputFile, const int p_trialId, const std::string& p_tick);
    void InsertGameState(std::ifstream& p_inputFile, const int p_trialId, const std::string& p_tick);
    void InsertUserInput(std::ifstream& p_inputFile, const int p_trialId, const std::string& p_tick);

    sql::Driver* m_driver;
    sql::Connection* m_connection;
    sql::Statement* m_statement;
    sql::ResultSet* m_resultSet;
};