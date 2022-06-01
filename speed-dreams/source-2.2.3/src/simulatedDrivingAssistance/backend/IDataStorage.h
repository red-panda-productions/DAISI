#pragma once

#include <string>
#include "FileSystem.hpp"

/// @brief An interface for classes that can store data
class IDataStorage
{
public:
    virtual void StoreData(const filesystem::path& p_filePath) = 0;
};