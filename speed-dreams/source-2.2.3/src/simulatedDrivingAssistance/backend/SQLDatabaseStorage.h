#pragma once
#include "IDataStorage.h"
#include <string>
#include <unordered_map>

/// @brief A class that can store data to a SQL database
class SQLDatabaseStorage : IDataStorage
{
public:
    SQLDatabaseStorage();
    void StoreData(const std::string p_filePath) override;

private:
    std::unordered_map<std::string, std::string> m_headerTypes;
};