#pragma once

#include <fstream>

template <class DriveSituation>
class FileDataStorage {
 private:
    bool* m_saveSettings;
    std::ofstream m_outputStream;
 public:
    explicit FileDataStorage(bool* p_saveSettings);
    void Initialise(const std::string& p_fileName, const std::string& p_userId);
    void Shutdown();
    void Save(DriveSituation& p_situation, int p_timestamp);
};

#define SFileDataStorage FileDataStorage<DriveSituation>

