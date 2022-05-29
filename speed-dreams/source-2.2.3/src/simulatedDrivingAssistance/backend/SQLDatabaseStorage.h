#pragma once
#include "IDataStorage.h"
#include <string>
#include <unordered_map>
#include "MySQLHeaders.h"
#include <fstream>
#include "ConfigEnums.h"

/// @brief A class that can store data to a SQL database
class SQLDatabaseStorage : IDataStorage
{
public:
    SQLDatabaseStorage();
    void Run(const filesystem::path& p_inputFilePath, const std::string& p_dirPath = "");
    void StoreData(const filesystem::path& p_inputFilePath) override;
    bool OpenDatabase(DatabaseSettings p_dbSettings,
                      const std::string& p_dirPath = "");

    void CloseDatabase();

private:
    void CreateTables();
    int InsertInitialData(std::ifstream& p_inputFile);
    void InsertSimulationData(std::ifstream& p_inputFile, int p_trialId);
    void InsertDecisions(std::ifstream& p_inputFile, int p_trialId, const std::string& p_tick);
    void InsertGameState(std::ifstream& p_inputFile, int p_trialId, const std::string& p_tick);
    void InsertUserInput(std::ifstream& p_inputFile, int p_trialId, const std::string& p_tick);
    void PutKeys(sql::ConnectOptionsMap& p_connectionProperties, DatabaseSettings p_dbSettings);

    sql::Driver* m_driver;
    sql::Connection* m_connection;
    sql::Statement* m_statement;
    sql::ResultSet* m_resultSet;
};