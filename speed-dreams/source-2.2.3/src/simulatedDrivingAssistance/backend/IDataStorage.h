#pragma once

#include <string>
#include "FileSystem.hpp"
#include "ConfigEnums.h"

/// @brief An interface for classes that can store data
class IDataStorage
{
public:
    virtual bool StoreData(const tBufferPaths& p_bufferPaths) = 0;
};