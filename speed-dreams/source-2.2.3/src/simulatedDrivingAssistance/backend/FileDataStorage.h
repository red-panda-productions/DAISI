#pragma once

#include <fstream>
#include "DriveSituation.h"

template <class DriveSituation>
class FileDataStorage {
 private:
    /// @brief Boolean array to determine what to save and what not to save. Uses indices as in ConfigEnums.h
    bool* m_saveSettings;
    /// @brief Output filestream to write data to, should be initialised through @link FileDataStorage::Initialise
    std::ofstream m_outputStream;
 public:
    /// @brief Initialise the temporary data storage
    /// @param p_saveSettings Boolean array to determine what to save and what not to save. Uses indices as in ConfigEnums.h
    explicit FileDataStorage(bool* p_saveSettings);
    /// @brief Initialise the file data storage.
    /// End result: a file is created at the given filepath, and initial data is written to the file.
    /// @param p_fileName Path of the file to save.
    /// @param p_userId User ID of the current player.
    void Initialise(const std::string& p_fileName, const std::string& p_userId);
    /// @brief Shutdown the file data storage.
    /// End result: any possible final data is written and the file is released.
    void Shutdown();
    /// @brief Writes information about the current driving situation to the current file based on settings.
    /// @param p_situation The current driving situation to write data about.
    /// @param p_timestamp The current timestamp of the situation.
    void Save(DriveSituation& p_situation, int p_timestamp);
};

/// @brief Standard implementation of the file data storage
#define SFileDataStorage FileDataStorage<DriveSituation>

