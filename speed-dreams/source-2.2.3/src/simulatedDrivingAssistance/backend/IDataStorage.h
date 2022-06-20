/*
 * This program has been developed by students from the bachelor Computer Science at
 * Utrecht University within the Software Project course.
 * © Copyright Utrecht University (Department of Information and Computing Sciences)
 */

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