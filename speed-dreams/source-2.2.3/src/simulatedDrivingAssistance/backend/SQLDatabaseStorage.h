#pragma once
#include "IDataStorage.h"
#include <unordered_map>

class SQLDatabaseStorage : IDataStorage
{
public:
    SQLDatabaseStorage();
    void StoreData(const std::string p_filePath) override;

private:
    std::unordered_map<std::string, std::string> m_headerTypes;
};