#pragma once

#include <string>
#include "FileSystem.hpp"

/// @brief An interface for classes that can store data
class IDataStorage
{
public:
    virtual bool StoreData(const filesystem::path& p_filePath) = 0;
};