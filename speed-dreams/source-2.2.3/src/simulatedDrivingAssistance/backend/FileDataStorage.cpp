#include <ostream>
#include "FileDataStorage.h"

// Write a literal string to the stream, as the string in text format (without conversion)
#define WRITE_STRING_LIT(stream, string) stream << string << "\n"
// Write a variable string to the stream, as the string in text format (without conversion)
#define WRITE_STRING_VAR(stream, string) stream << string << "\n"
// Write a variable to the stream
#define WRITE_VAR(stream, val) stream << std::to_string(val) << "\n" //Binary: stream.write(reinterpret_cast<const char*>(&val), sizeof(val)); stream << "\n"

/// @brief Write a time variable to a stream as a DateTime entry (aka as a "YYYY-MM-DD hh:mm:ss" string),
///  finished by a newline.
/// @param stream Output stream to write time to.
/// @param date Time to format and write to the stream.
inline void writeTime(std::ostream& stream, time_t date) {
    // "YYYY-MM-DD hh:mm:ss" is 19 characters, finishing with a nullpointer makes 20.
    // Thus allocate space for 20 characters.
    char buffer[20];
    strftime(buffer, 20, "%F %T", gmtime(&date));
    stream << buffer << "\n";
}

/// @brief Initialise the file data storage.
/// End result: a file is created at the given filepath, and initial data is written to the file.
/// @param p_fileName Path of the file to save.
/// @param p_userId User ID of the current player.
/// @param p_trialStartTime Start time of the current race
/// @param p_blackboxFilename Filename without path (e.g. "blackbox.exe") for the current black box
/// @param p_blackboxName Name of the current black box (e.g. "Client Black Box")
/// @param p_blackboxTime Timestamp for when the current black box was last updated
/// @param p_environmentFilename Filename without path (e.g. "highway.xml") for the current environment
/// @param p_environmentName Name of the current environment (e.g. "Espie Circuit")
/// @param p_environmentVersion Version of the current environment
/// @param p_interventionType Intervention type for the current race
void FileDataStorage::Initialise(const std::string& p_fileName,
                                 const std::string& p_userId,
                                 const std::time_t& p_trialStartTime,
                                 const std::string& p_blackboxFilename,
                                 const std::string& p_blackboxName,
                                 const std::time_t& p_blackboxTime,
                                 const std::string& p_environmentFilename,
                                 const std::string& p_environmentName,
                                 int p_environmentVersion,
                                 InterventionType p_interventionType
) {
    m_outputStream.open(p_fileName);
    // User and trial data
    WRITE_STRING_VAR(m_outputStream, p_userId);
    writeTime(m_outputStream, p_trialStartTime);
    // Black box data
    WRITE_STRING_VAR(m_outputStream, p_blackboxFilename);
    writeTime(m_outputStream, p_blackboxTime);
    WRITE_STRING_VAR(m_outputStream, p_blackboxName);
    // Environment data
    WRITE_STRING_VAR(m_outputStream, p_environmentFilename);
    WRITE_VAR(m_outputStream, p_environmentVersion);
    WRITE_STRING_VAR(m_outputStream, p_environmentName);
    // Intervention data
    WRITE_VAR(m_outputStream, p_interventionType);
    // Headers to indicate what data will be saved
    if (m_saveSettings.EnvironmentData) {
        WRITE_STRING_LIT(m_outputStream, "GameState");
    }
    if (m_saveSettings.HumanData) {
        WRITE_STRING_LIT(m_outputStream, "UserInput");
    }
}

/// @brief Shutdown the file data storage.
/// End result: any possible final data is written and the file is released.
void FileDataStorage::Shutdown() {
    m_outputStream.close();
}

/// @brief Save the current driving situation to the buffer
/// @param p_car Current car status in Speed Dreams
/// @param p_situation Current situation in Speed Dreams
/// @param p_timestamp Current tick
void FileDataStorage::Save(tCarElt* p_car, tSituation* p_situation, unsigned long p_timestamp) {
    WRITE_VAR(m_outputStream, p_timestamp);
    if (m_saveSettings.EnvironmentData) {
        Posd pos = p_car->pub.DynGCg.pos;
        tDynPt mov = p_car->pub.DynGC;
        WRITE_VAR(m_outputStream, pos.x); // x-position
        WRITE_VAR(m_outputStream, pos.y); // y-position
        WRITE_VAR(m_outputStream, pos.z); // z-position
        WRITE_VAR(m_outputStream, pos.ax);// x-direction
        WRITE_VAR(m_outputStream, pos.ay);// y-direction
        WRITE_VAR(m_outputStream, pos.az);// z-direction
        WRITE_VAR(m_outputStream, mov.vel.x); // speed
        WRITE_VAR(m_outputStream, mov.acc.x); // acceleration
        WRITE_VAR(m_outputStream, p_car->priv.gear); // gear
    }
    if (m_saveSettings.HumanData) {
        tCarCtrl ctrl = p_car->ctrl;
        WRITE_VAR(m_outputStream, ctrl.steer); // steer
        WRITE_VAR(m_outputStream, ctrl.brakeCmd); // brake
        WRITE_VAR(m_outputStream, ctrl.accelCmd); // gas
        WRITE_VAR(m_outputStream, ctrl.clutchCmd); // clutch
    }
}

/// @brief Initialise the temporary data storage.
/// @param p_saveSettings Settings for what data to store.
FileDataStorage::FileDataStorage(tDataToStore p_saveSettings) : m_saveSettings(p_saveSettings) {

}

/// @brief Save all decisions that were taken this tick
/// @param p_decisions Tuple of decisions taken this tick
void FileDataStorage::SaveDecisions(DecisionTuple& p_decisions) {
    if (!m_saveSettings.InterventionData) return;
    WRITE_STRING_LIT(m_outputStream, "Decisions");
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
    WRITE_STRING_LIT(m_outputStream, "NONE");
}

