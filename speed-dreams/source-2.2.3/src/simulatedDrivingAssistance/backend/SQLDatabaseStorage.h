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
    void Run(const std::experimental::filesystem::path& p_inputFilePath, const std::string& p_dirPath = "");
    void StoreData(const std::experimental::filesystem::path& p_inputFilePath) override;
    bool OpenDatabase(const std::string& p_hostName,
                      int p_port,
                      const std::string& p_username,
                      const std::string& p_password,
                      const std::string& p_schemaName,
                      bool p_useEncryption,
                      const std::string& p_dirPath = "");

    void CloseDatabase();

private:
    void CreateTables();
    int InsertInitialData(std::ifstream& p_inputFile);
    void InsertSimulationData(std::ifstream& p_inputFile, int p_trialId);
    void InsertDecisions(std::ifstream& p_inputFile, int p_trialId, const std::string& p_tick);
    void InsertGameState(std::ifstream& p_inputFile, int p_trialId, const std::string& p_tick);
    void InsertUserInput(std::ifstream& p_inputFile, int p_trialId, const std::string& p_tick);
    void PutKeys(const std::string& p_dirPath, sql::ConnectOptionsMap& p_connectionProperties);

    sql::Driver* m_driver;
    sql::Connection* m_connection;
    sql::Statement* m_statement;
    sql::ResultSet* m_resultSet;
};