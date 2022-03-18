#include <string>
#include "FileDataStorage.h"
FileDataStorage::FileDataStorage(bool* p_saveSettings) : m_saveSettings(p_saveSettings) {}

void FileDataStorage::Initialise(const std::string& p_fileName, const std::string& p_userId) {
    m_outputStream.open(p_fileName);
    m_outputStream << "USER:" + p_userId;
}
void FileDataStorage::Shutdown() {
    m_outputStream.close();
}
void FileDataStorage::Save(DriveSituation* p_situation) {
    int timestep = 0; // TODO: get timestep from situation

    m_outputStream << "START:" + std::to_string(timestep) + "\n";
    // TODO: write situation data
    m_outputStream << "END:" + std::to_string(timestep) + "\n";
}
