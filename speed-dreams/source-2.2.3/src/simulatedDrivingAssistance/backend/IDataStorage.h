#pragma once

class IDataStorage
{
public:
    virtual void StoreData(const std::string p_filePath) = 0;
};