#pragma once
#include <string>
#include "ConfigEnums.h"
#include <time.h>

/// @brief Creates an implementation of file data storage
#define CREATE_FILE_DATA_STORAGE_IMPLEMENTATION(type)\
    template void FileDataStorage<type>::Initialise(const std::string& p_fileName, \
        const std::string& p_userId,                        \
        const std::time_t& p_trialStartTime,                \
        const std::string& p_blackboxFilename,              \
        const std::string& p_blackboxName,                  \
        const std::time_t& p_blackboxTime,                  \
        const std::string& p_environmentFilename,           \
        const std::string& p_environmentName,               \
        const std::time_t& p_environmentTime,               \
        InterventionType   p_interventionType               \
        );                                                  \
    template void FileDataStorage<type>::Shutdown();\
    template void FileDataStorage<type>::Save(type& p_situation, const DecisionTuple& decisions, int p_timestamp);

// Write a literal string to the stream, as the string in text format (without conversion)
#define WRITE_STRING_LIT(stream, string) stream << string << "\n"
// Write a variable string to the stream, as the string in text format (without conversion)
#define WRITE_STRING_VAR(stream, string) stream << string << "\n"
// Write a variable to the stream
#define WRITE_VAR(stream, val) stream << std::to_string(val) << "\n" //Binary: stream.write(reinterpret_cast<const char*>(&val), sizeof(val)); stream << "\n"
// Write a time_t variable to the stream as a DateTime entry (aka as a "YYYY-MM-DD hh:mm:ss" string)
// "YYYY-MM-DD hh:mm:ss" is 19 characters, finishing with a nullpointer makes 20. Thus allocate space for 20 characters.
// Use {brackets} to ensure scope of buffer variable stays limited
#define WRITE_TIME(stream, date) { char buffer[20]; \
    strftime(buffer, 20, "%F %T", gmtime(&date)); \
    stream << buffer << "\n"; }

/// @brief Initialise the file data storage.
/// End result: a file is created at the given filepath, and initial data is written to the file.
/// @param p_fileName Path of the file to save.
/// @param p_userId User ID of the current player.
template<class BlackBoxData>
void FileDataStorage<BlackBoxData>::Initialise(const std::string& p_fileName,
                                               const std::string& p_userId,
                                               const std::time_t& p_trialStartTime,
                                               const std::string& p_blackboxFilename,
                                               const std::string& p_blackboxName,
                                               const std::time_t& p_blackboxTime,
                                               const std::string& p_environmentFilename,
                                               const std::string& p_environmentName,
                                               const std::time_t& p_environmentTime,
                                               InterventionType p_interventionType
) {
    m_outputStream.open(p_fileName);
    // User and trial data
    WRITE_STRING_VAR(m_outputStream, p_userId);
    WRITE_TIME(m_outputStream, p_trialStartTime);
    // Black box data
    WRITE_STRING_VAR(m_outputStream, p_blackboxFilename);
    WRITE_TIME(m_outputStream, p_blackboxTime);
    WRITE_STRING_VAR(m_outputStream, p_blackboxName);
    // Environment data
    WRITE_STRING_VAR(m_outputStream, p_environmentFilename);
    WRITE_TIME(m_outputStream, p_environmentTime);
    WRITE_STRING_VAR(m_outputStream, p_environmentName);
    // Intervention data
    WRITE_VAR(m_outputStream, p_interventionType);
    // Headers to indicate what data will be saved
    if (m_saveSettings->EnvironmentData) {
        WRITE_STRING_LIT(m_outputStream, "GameState");
    }
    if (m_saveSettings->HumanData) {
        WRITE_STRING_LIT(m_outputStream, "UserInput");
    }
    if (m_saveSettings->InterventionData) {
        WRITE_STRING_LIT(m_outputStream, "Decisions");
    }
}

/// @brief Shutdown the file data storage.
/// End result: any possible final data is written and the file is released.
template<class BlackBoxData>
void FileDataStorage<BlackBoxData>::Shutdown() {
    m_outputStream.close();
}
/// @brief Writes information about the current driving situation to the current file based on settings.
/// @param p_data The current driving situation to write data about.
/// @param p_timestamp The current timestamp of the situation.
template<class BlackBoxData>
void FileDataStorage<BlackBoxData>::Save(BlackBoxData& p_data, const DecisionTuple& p_decisions, int p_timestamp) {
    WRITE_VAR(m_outputStream, p_timestamp);
    if (m_saveSettings->EnvironmentData) {
        Posd pos = p_data.Car.pub.DynGCg.pos;
        tDynPt mov = p_data.Car.pub.DynGC;
        WRITE_VAR(m_outputStream, pos.x); // x-position
        WRITE_VAR(m_outputStream, pos.y); // y-position
        WRITE_VAR(m_outputStream, pos.z); // z-position
        WRITE_VAR(m_outputStream, pos.ax);// x-direction
        WRITE_VAR(m_outputStream, pos.ay);// y-direction
        WRITE_VAR(m_outputStream, pos.az);// z-direction
        WRITE_VAR(m_outputStream, mov.vel.x); // speed
        WRITE_VAR(m_outputStream, mov.acc.x); // acceleration
        WRITE_VAR(m_outputStream, p_data.Car.priv.gear); // gear
    }
    if (m_saveSettings->HumanData) {
        tCarCtrl ctrl = p_data.Car.ctrl;
        WRITE_VAR(m_outputStream, ctrl.steer); // steer
        WRITE_VAR(m_outputStream, ctrl.brakeCmd); // brake
        WRITE_VAR(m_outputStream, ctrl.accelCmd); // gas
        WRITE_VAR(m_outputStream, ctrl.clutchCmd); // clutch
    }
    if (m_saveSettings->InterventionData) {
        if (p_decisions.GetContainsSteer()) {
            WRITE_STRING_LIT(m_outputStream, "SteerDecision");
            WRITE_VAR(m_outputStream, p_decisions.GetSteer());
        }
        if (p_decisions.GetContainsBrake()) {
            WRITE_STRING_LIT(m_outputStream, "BrakeDecision");
            WRITE_VAR(m_outputStream, p_decisions.GetBrake());
        }
        if (p_decisions.GetContainsAccel()) {
            WRITE_STRING_LIT(m_outputStream, "AccelDecision");
            WRITE_VAR(m_outputStream, p_decisions.GetAccel());
        }
        if (p_decisions.GetContainsGear()) {
            WRITE_STRING_LIT(m_outputStream, "GearDecision");
            WRITE_VAR(m_outputStream, p_decisions.GetGear());
        }
        if (p_decisions.GetContainsLights()) {
            WRITE_STRING_LIT(m_outputStream, "LightsDecision");
            WRITE_VAR(m_outputStream, p_decisions.GetLights());
        }
        WRITE_STRING_LIT(m_outputStream, "NONE"); // Write that there are no (more) decisions
    }
}

/// @brief Initialise the temporary data storage
/// @param p_saveSettings Boolean array to determine what to save and what not to save. Uses indices as in ConfigEnums.h
template<class DriveSituation>
FileDataStorage<DriveSituation>::FileDataStorage(tDataToStore* p_saveSettings) :m_saveSettings(p_saveSettings) {

};
