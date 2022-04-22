#pragma once

#include <string>

#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING 1

#include <experimental/filesystem>

/// @brief An interface for classes that can store data
class IDataStorage
{
public:
    virtual void StoreData(const std::experimental::filesystem::path& p_filePath) = 0;
};