#pragma once

#include <fstream>
#include "DriveSituation.h"

/// @brief                 A class that can store data to a file
/// @tparam DriveSituation The drive situation that needs to be stored
template <class DriveSituation>
class FileDataStorage {
 private:
    /// @brief Boolean array to determine what to save and what not to save. Uses indices as in ConfigEnums.h
    bool* m_saveSettings;
    /// @brief Output filestream to write data to, should be initialised through @link FileDataStorage::Initialise
    std::ofstream m_outputStream;
 public:
    FileDataStorage(bool* p_saveSettings);

    void Initialise(const std::string& p_fileName, const std::string& p_userId);

    void Shutdown();

    void Save(DriveSituation& p_situation, int p_timestamp);
};

/// @brief Standard implementation of the file data storage
#define SFileDataStorage FileDataStorage<DriveSituation>

