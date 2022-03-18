#pragma once
#include <string>
#include "ConfigEnums.h"

#define CREATE_FILE_DATA_STORAGE_IMPLEMENTATION(type);\
    template void FileDataStorage<type>::Initialise(const std::string& p_fileName, const std::string& p_userId);\
    template void FileDataStorage<type>::Shutdown();\
    template void FileDataStorage<type>::Save(DriveSituation& p_situation, int p_timestamp);

// Write a literal string to the stream, as the string in text format (without conversion)
#define WRITE_STRING_LIT(stream, string) stream << string << "\n"
// Write a variable string to the stream, as the string in text format (without conversion)
#define WRITE_STRING_VAR(stream, string) stream << string << "\n"
// Write a variable to the stream
#define WRITE_VAR(stream, val) stream << std::to_string(val) << "\n" //Binary: stream.write(reinterpret_cast<const char*>(&val), sizeof(val)); stream << "\n"

void WriteEnvironmentHeaders(std::ostream& p_outputStream) {
    WRITE_STRING_LIT(p_outputStream, "TimeOfDay");
    WRITE_STRING_LIT(p_outputStream, "Clouds");
    WRITE_STRING_LIT(p_outputStream, "Rain");
}
template<class EnvironmentInfo>
void WriteEnvironmentData(std::ostream& p_outputStream, EnvironmentInfo& p_envInfo) {
    WRITE_VAR(p_outputStream, p_envInfo.TimeOfDay());
    WRITE_VAR(p_outputStream, p_envInfo.Clouds());
    WRITE_VAR(p_outputStream, p_envInfo.Rain());
}
void WriteCarHeaders(std::ostream& p_outputStream) {
    WRITE_STRING_LIT(p_outputStream, "Speed");
    WRITE_STRING_LIT(p_outputStream, "Gear");
    WRITE_STRING_LIT(p_outputStream, "Headlights");
    WRITE_STRING_LIT(p_outputStream, "Offroad");
}
template<class CarInfo>
void WriteCarData(std::ostream& p_outputStream, CarInfo& p_carInfo) {
    WRITE_VAR(p_outputStream, p_carInfo.Speed());
    WRITE_VAR(p_outputStream, p_carInfo.Gear());
    WRITE_VAR(p_outputStream, p_carInfo.Headlights());
    WRITE_VAR(p_outputStream, p_carInfo.TrackLocalPosition()->Offroad());
}
void WritePlayerHeaders(std::ostream& p_outputStream) {
    WRITE_STRING_LIT(p_outputStream, "AccelCmd");
    WRITE_STRING_LIT(p_outputStream, "BrakeCmd");
    WRITE_STRING_LIT(p_outputStream, "ClutchCmd");
    WRITE_STRING_LIT(p_outputStream, "SteerCmd");
}
template<class PlayerInfo>
void WritePlayerData(std::ostream& p_outputStream, PlayerInfo& p_playerInfo) {
    WRITE_VAR(p_outputStream, p_playerInfo.AccelCmd());
    WRITE_VAR(p_outputStream, p_playerInfo.BrakeCmd());
    WRITE_VAR(p_outputStream, p_playerInfo.ClutchCmd());
    WRITE_VAR(p_outputStream, p_playerInfo.SteerCmd());
}

template<class DriveSituation>
void FileDataStorage<DriveSituation>::Initialise(const std::string& p_fileName, const std::string& p_userId) {
    m_outputStream.open(p_fileName);
    WRITE_STRING_VAR(m_outputStream, p_userId);

    if (m_saveSettings[DATA_TO_STORE_ENVIRONMENT_DATA]) {
        WriteEnvironmentHeaders(m_outputStream);
    }
    if (m_saveSettings[DATA_TO_STORE_CAR_DATA]) {
        WriteCarHeaders(m_outputStream);
    }
    if (m_saveSettings[DATA_TO_STORE_HUMAN_DATA]) {
        WritePlayerHeaders(m_outputStream);
    }
    if (m_saveSettings[DATA_TO_STORE_INTERVENTION_DATA]) {
        // TODO: write intervention headers here
    }
    if (m_saveSettings[DATA_TO_STORE_META_DATA]) {
        // TODO: write metadata headers here
    }
}

template<class DriveSituation>
void FileDataStorage<DriveSituation>::Shutdown() {
    m_outputStream.close();
}

template<class DriveSituation>
void FileDataStorage<DriveSituation>::Save(DriveSituation& p_situation, int p_timestamp) {
    WRITE_VAR(m_outputStream, p_timestamp);

    if (m_saveSettings[DATA_TO_STORE_ENVIRONMENT_DATA]) {
        WriteEnvironmentData(m_outputStream, *p_situation.GetEnvironmentInfo());
    }
    if (m_saveSettings[DATA_TO_STORE_CAR_DATA]) {
        WriteCarData(m_outputStream, *p_situation.GetCarInfo());
    }
    if (m_saveSettings[DATA_TO_STORE_HUMAN_DATA]) {
        WritePlayerData(m_outputStream, *p_situation.GetPlayerInfo());
    }
    if (m_saveSettings[DATA_TO_STORE_INTERVENTION_DATA]) {
        // TODO: write intervention data here
    }
    if (m_saveSettings[DATA_TO_STORE_META_DATA]) {
        // TODO: write metadata data here
    }
}

template<class DriveSituation>
FileDataStorage<DriveSituation>::FileDataStorage(bool* p_saveSettings):m_saveSettings(p_saveSettings) {}