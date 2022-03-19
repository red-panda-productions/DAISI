#pragma once

#include <fstream>
#include "DriveSituation.h"

template <class DriveSituation>
class FileDataStorage {
 private:
    /// @brief Boolean array to determine what to save and what not to save. Uses indices as in
    bool* m_saveSettings;
    std::ofstream m_outputStream;
 public:
    explicit FileDataStorage(bool* p_saveSettings);
    void Initialise(const std::string& p_fileName, const std::string& p_userId);
    void Shutdown();
    void Save(DriveSituation& p_situation, int p_timestamp);
};

#define SFileDataStorage FileDataStorage<DriveSituation>

