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
    SQLDatabaseStorageMock()
        : SQLDatabaseStorageMock({true, true, true, true}) {}

    explicit SQLDatabaseStorageMock(tDataToStore p_dataToStore)
    {
        const auto dataToStore = new tDataToStore(p_dataToStore);

        VariableStore::GetInstance().Variables[0] = static_cast<void*>(dataToStore);
    }

    void Run(const tBufferPaths& p_bufferPaths)
    {
        const auto bufferPaths = new tBufferPaths(p_bufferPaths);

        VariableStore::GetInstance().Variables[1] = static_cast<void*>(bufferPaths);
    }

    bool StoreData(const tBufferPaths& p_bufferPaths) override  // @NOCOVERAGE, This function definition is needed for building (IDataStorage is abstract), but will not be used and therefore not covered.
    {                                                           // @NOCOVERAGE
        return true;                                            // @NOCOVERAGE
    }                                                           // @NOCOVERAGE

    bool OpenDatabase(const std::string& p_hostName,
                      int p_port,
                      const std::string& p_username,
                      const std::string& p_password,
                      const std::string& p_schemaName) {}

    void CloseDatabase() {}
};