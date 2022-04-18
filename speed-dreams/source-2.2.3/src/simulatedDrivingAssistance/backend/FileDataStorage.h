#pragma once

#include <fstream>
#include "BlackBoxData.h"
#include "ConfigEnums.h"

/// @brief               A class that can store data to a file
/// @tparam BlackBoxData The data that needs to be stored
template <class BlackBoxData>
class FileDataStorage
{
private:
    /// @brief Boolean array to determine what to save and what not to save. Uses indices as in ConfigEnums.h
    tDataToStore* m_saveSettings;
    /// @brief Output filestream to write data to, should be initialised through @link FileDataStorage::Initialise
    std::ofstream m_outputStream;

public:
    explicit FileDataStorage(tDataToStore* p_saveSettings);

    void Initialise(const std::string& p_fileName, const std::string& p_userId);

    void Shutdown();

    void Save(BlackBoxData& p_data, int p_timestamp);
};

/// @brief Standard implementation of the file data storage
#define SFileDataStorage FileDataStorage<BlackBoxData>
