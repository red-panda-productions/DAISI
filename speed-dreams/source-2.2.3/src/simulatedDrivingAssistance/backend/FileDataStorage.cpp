#include <string>
#include "FileDataStorage.h"
#define WRITE_STRING(stream, string) stream << string + "\n"
#define WRITE_VAR(stream, val) stream << std::to_string(val) + "\n"//Binary: stream.write(reinterpret_cast<const char*>(&val), sizeof(val)); stream << "\n"

FileDataStorage::FileDataStorage(bool* p_saveSettings) : m_saveSettings(p_saveSettings) {}

void FileDataStorage::Initialise(const std::string& p_fileName, const std::string& p_userId) {
    m_outputStream.open(p_fileName);
    WRITE_STRING(m_outputStream, p_userId);
}
void FileDataStorage::Shutdown() {
    m_outputStream.close();
}
void FileDataStorage::Save(DriveSituation* p_situation, int p_timestamp) {
    WRITE_VAR(m_outputStream, p_timestamp);
    // TODO: write situation data
}
