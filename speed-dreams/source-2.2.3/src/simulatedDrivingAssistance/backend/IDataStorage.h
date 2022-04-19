#pragma once
#include <string>

/// @brief An interface for classes that can store data
class IDataStorage
{
public:
    virtual void StoreData(std::string p_filePath) = 0;
};