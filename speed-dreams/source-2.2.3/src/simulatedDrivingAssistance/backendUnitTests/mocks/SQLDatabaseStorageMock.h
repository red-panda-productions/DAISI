#pragma once
#include "IDataStorage.h"
#include <string>
#include <unordered_map>
#include <fstream>
#include "MySQLHeaders.h"
#include "VariableStore.h"

/// @brief A class that can store data to a SQL database
class SQLDatabaseStorageMock : IDataStorage
{
public:
    SQLDatabaseStorageMock(){}

    SQLDatabaseStorageMock(tDataToStore p_dataToStore)
        : SQLDatabaseStorageMock() {}

    void Run(const tBufferPaths& p_bufferPaths)
    {
        /*InputFilePath = p_inputFilePath;

        const auto path = new filesystem::path(p_inputFilePath);

        VariableStore::GetInstance().Variables[0] = static_cast<void*>(path);*/
    }

    void StoreData(const tBufferPaths& p_bufferPaths) override  //@NOCOVERAGE, This function is needed for building but is never called
    {
    }

    bool OpenDatabase(const std::string& p_hostName,
                      int p_port,
                      const std::string& p_username,
                      const std::string& p_password,
                      const std::string& p_schemaName){};

    void CloseDatabase() {}
};