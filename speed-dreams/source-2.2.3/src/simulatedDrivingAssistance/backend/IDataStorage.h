#pragma once

/// @brief An interface for classes that can store data
class IDataStorage
{
public:
    virtual void StoreData(const std::string p_filePath) = 0;
};