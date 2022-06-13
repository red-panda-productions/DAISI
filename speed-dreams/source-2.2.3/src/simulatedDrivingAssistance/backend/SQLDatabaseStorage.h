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
    explicit SQLDatabaseStorage(tDataToStore p_dataToStore);

    bool TestConnection(DatabaseSettings p_dbSettings);
    bool Run(const tBufferPaths& p_bufferPaths);
    bool StoreData(const tBufferPaths& p_bufferPaths) override;
    bool OpenDatabase(DatabaseSettings p_dbSettings);
    bool CloseDatabase(bool p_returnVal);

    static void PutKeys(sql::ConnectOptionsMap& p_connectionProperties, DatabaseSettings p_dbSettings);
private:
    void CreateDatabaseSchema(DatabaseSettings p_dbSettings);
    void CreateTables();

    int InsertMetaData(std::ifstream& p_inputFileStream);
    void InsertSimulationData(const tBufferPaths& p_bufferPaths, int p_trialId);
    void InsertGameState(const filesystem::path& p_gameStatePath, int p_trialId);
    void InsertUserInput(const filesystem::path& p_userInputPath, int p_trialId);
    void InsertDecisions(const filesystem::path& p_decisionsPath, int p_trialId);

    std::vector<std::string> GetMissingPrivileges(DatabaseSettings p_dbSettings);

    sql::Driver* m_driver;
    sql::Connection* m_connection;
    sql::Statement* m_statement;
    sql::ResultSet* m_resultSet;

    tDataToStore m_dataToStore;
    bool ConnectDatabase(DatabaseSettings p_dbSettings);
};