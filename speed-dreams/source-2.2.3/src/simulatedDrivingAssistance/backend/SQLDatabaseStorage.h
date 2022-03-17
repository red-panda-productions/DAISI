#pragma once
#include "IDataStorage.h"

class SQLDatabaseStorage : IDataStorage
{
public:
    void StoreData() override;
};