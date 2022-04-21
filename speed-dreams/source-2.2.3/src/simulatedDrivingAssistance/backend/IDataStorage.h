#pragma once
#include <string>
#include <experimental/filesystem>

/// @brief An interface for classes that can store data
class IDataStorage
{
public:
    virtual void StoreData(const std::experimental::filesystem::path& p_filePath) = 0;
};