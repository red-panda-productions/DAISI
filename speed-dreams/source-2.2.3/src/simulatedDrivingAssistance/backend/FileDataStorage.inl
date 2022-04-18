#pragma once
#include <string>
#include "ConfigEnums.h"

/// @brief Creates an implementation of file data storage
#define CREATE_FILE_DATA_STORAGE_IMPLEMENTATION(type)                                                            \
    template void FileDataStorage<type>::Initialise(const std::string& p_fileName, const std::string& p_userId); \
    template void FileDataStorage<type>::Shutdown();                                                             \
    template void FileDataStorage<type>::Save(type& p_situation, int p_timestamp);

// Write a literal string to the stream, as the string in text format (without conversion)
#define WRITE_STRING_LIT(stream, string) stream << (string) << "\n"
// Write a variable string to the stream, as the string in text format (without conversion)
#define WRITE_STRING_VAR(stream, string) stream << (string) << "\n"
// Write a variable to the stream
#define WRITE_VAR(stream, val) stream << std::to_string(val) << "\n"  // Binary: stream.write(reinterpret_cast<const char*>(&val), sizeof(val)); stream << "\n"

/// @brief Write headers for the environment data in the same order as actual data will be written
/// @param p_outputStream stream to write headers to
void WriteEnvironmentHeaders(std::ostream& p_outputStream)
{
    WRITE_STRING_LIT(p_outputStream, "TimeOfDay");
    WRITE_STRING_LIT(p_outputStream, "Clouds");
    WRITE_STRING_LIT(p_outputStream, "Rain");
}
/// @brief Write data about the current environment to the file.
/// @tparam BlackBoxData Type of Environment Info to use, should contain the proper definitions.
/// @param p_outputStream The stream to write data to.
/// @param p_data          The current data information.
template <class BlackBoxData>
void WriteEnvironmentData(std::ostream& p_outputStream, BlackBoxData& p_data)
{
    WRITE_VAR(p_outputStream, 0);  // are set in the track not in the black box data
    WRITE_VAR(p_outputStream, 0);
    WRITE_VAR(p_outputStream, 0);
}
/// @brief Write headers for the car data in the same order as actual data will be written
/// @param p_outputStream stream to write headers to
inline void WriteCarHeaders(std::ostream& p_outputStream)
{
    WRITE_STRING_LIT(p_outputStream, "Speed");
    WRITE_STRING_LIT(p_outputStream, "Gear");
    WRITE_STRING_LIT(p_outputStream, "Headlights");
    WRITE_STRING_LIT(p_outputStream, "Offroad");
}
/// @brief Write data about the current car status to the file.
/// @tparam BlackBoxData Type of Car Info to use, should contain the proper definitions.
/// @param p_outputStream The stream to write data to.
/// @param p_data The current car status information.
template <class BlackBoxData>
void WriteCarData(std::ostream& p_outputStream, BlackBoxData& p_data)
{
    WRITE_VAR(p_outputStream, p_data.Car._speed_x * 3.6f);
    WRITE_VAR(p_outputStream, p_data.Car._gear);
    WRITE_VAR(p_outputStream, false);
    WRITE_VAR(p_outputStream, false);
}
/// @brief Write headers for the player data in the same order as actual data will be written
/// @param p_outputStream stream to write headers to
inline void WritePlayerHeaders(std::ostream& p_outputStream)
{
    WRITE_STRING_LIT(p_outputStream, "AccelCmd");
    WRITE_STRING_LIT(p_outputStream, "BrakeCmd");
    WRITE_STRING_LIT(p_outputStream, "ClutchCmd");
    WRITE_STRING_LIT(p_outputStream, "SteerCmd");
}
/// @brief Write data about the current player control status to the file.
/// @tparam BlackBoxData Type of Player Info to use, should contain the proper definitions.
/// @param p_outputStream The stream to write data to.
/// @param p_data         The current player control information.
template <class BlackBoxData>
void WritePlayerData(std::ostream& p_outputStream, BlackBoxData& p_data)
{
    WRITE_VAR(p_outputStream, p_data.Car._accelCmd);
    WRITE_VAR(p_outputStream, p_data.Car._brakeCmd);
    WRITE_VAR(p_outputStream, p_data.Car._clutchCmd);
    WRITE_VAR(p_outputStream, p_data.Car._steerCmd);
}

/// @brief Initialise the file data storage.
/// End result: a file is created at the given filepath, and initial data is written to the file.
/// @param p_fileName Path of the file to save.
/// @param p_userId User ID of the current player.
template <class BlackBoxData>
void FileDataStorage<BlackBoxData>::Initialise(const std::string& p_fileName, const std::string& p_userId)
{
    m_outputStream.open(p_fileName);
    WRITE_STRING_VAR(m_outputStream, p_userId);

    if (m_saveSettings->EnvironmentData)
    {
        WriteEnvironmentHeaders(m_outputStream);
    }
    if (m_saveSettings->CarData)
    {
        WriteCarHeaders(m_outputStream);
    }
    if (m_saveSettings->HumanData)
    {
        WritePlayerHeaders(m_outputStream);
    }
    if (m_saveSettings->InterventionData)
    {
        // TODO: write intervention headers here
    }
    if (m_saveSettings->MetaData)
    {
        // TODO: write metadata headers here
    }
}

/// @brief Shutdown the file data storage.
/// End result: any possible final data is written and the file is released.
template <class BlackBoxData>
void FileDataStorage<BlackBoxData>::Shutdown()
{
    m_outputStream.close();
}
/// @brief Writes information about the current driving situation to the current file based on settings.
/// @param p_data The current driving situation to write data about.
/// @param p_timestamp The current timestamp of the situation.
template <class BlackBoxData>
void FileDataStorage<BlackBoxData>::Save(BlackBoxData& p_data, int p_timestamp)
{
    WRITE_VAR(m_outputStream, p_timestamp);

    if (m_saveSettings->EnvironmentData)
    {
        WriteEnvironmentData(m_outputStream, p_data);
    }
    if (m_saveSettings->CarData)
    {
        WriteCarData(m_outputStream, p_data);
    }
    if (m_saveSettings->HumanData)
    {
        WritePlayerData(m_outputStream, p_data);
    }
    if (m_saveSettings->InterventionData)
    {
        // TODO: write intervention data here
    }
    if (m_saveSettings->MetaData)
    {
        // TODO: write metadata data here
    }
}

/// @brief Initialise the temporary data storage
/// @param p_saveSettings Boolean array to determine what to save and what not to save. Uses indices as in ConfigEnums.h
template <class DriveSituation>
FileDataStorage<DriveSituation>::FileDataStorage(tDataToStore* p_saveSettings)
    : m_saveSettings(p_saveSettings){

      };
