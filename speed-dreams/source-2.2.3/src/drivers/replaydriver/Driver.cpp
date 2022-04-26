#include "Driver.h"
#include "Mediator.h"
#include "ConfigEnums.h"
#include <tgf.h>
#include <Recorder.h>

#include "../../simulatedDrivingAssistance/rppUtils/RppUtils.hpp"

#define RECORDING_NAME "userRecording20220426-140251"

/// @brief Initialize the driver with the given track
/// Make sure the human driver is initialized and ready to drive.
/// @param p_index The driver's index (starting from 1)
/// @param p_name The driver's name
Driver::Driver(int p_index, const char* p_name) : m_index(p_index) {
}

/// @brief Initialize the driver with the given track.
/// Also notify the mediator that the race starts.
/// @param p_track The track that is being initialized
/// @param p_carHandle
/// @param p_carParmHandle
/// @param p_situation The current race situation
void Driver::InitTrack(tTrack* p_track, void* p_carHandle, void** p_carParmHandle, tSituation* p_situation) {
    std::experimental::filesystem::path sdaFolder;
    if (!GetSdaFolder(sdaFolder)) return;
    sdaFolder.append("user_recordings").append(RECORDING_NAME);

    *p_carParmHandle =
        GfParmReadFile(std::experimental::filesystem::path(sdaFolder).append(CAR_SETTINGS_FILE_NAME).string().c_str(),
                       GFPARM_RMODE_STD,
                       true);

    m_replayFile.open(sdaFolder.append(USER_INPUT_RECORDING_FILE_NAME).string().c_str());
    SMediator::GetInstance()->RaceStart(p_track, p_carHandle, p_carParmHandle, p_situation);
}

/// @brief Start a new race.
/// it opens te replay file and sets the time of the first input
/// @param p_car The car the driver controls
/// @param p_situation The current race situation
void Driver::NewRace(tCarElt* p_car, tSituation* p_situation) {
    std::string inputTime;
    m_replayFile >> inputTime;
    m_inputTime = std::stod(inputTime);
}

/// @brief Update the car's controls based on recording at that currentTime.
/// It then gets input from the black-box, which changes the input as well
/// It ends the race if the file is done
/// @param p_car The car the driver controls
/// @param p_situation The current race situation
void Driver::Drive(tCarElt* p_car, tSituation* p_situation) {
    std::string accelString;
    m_replayFile >> accelString;
    float accel = std::stof(accelString);
    std::string brakeString;
    m_replayFile >> brakeString;
    float brake = std::stof(brakeString);
    std::string steerString;
    m_replayFile >> steerString;
    float steer = std::stof(steerString);
    std::string gearString;
    m_replayFile >> gearString;
    float gear = std::stof(gearString);
    std::string clutchString;
    m_replayFile >> clutchString;
    float clutch = std::stof(clutchString);

    std::string raceCmdString;
    m_replayFile >> raceCmdString;
    float raceCmd = std::stof(raceCmdString);
    std::string lightCmdString;
    m_replayFile >> lightCmdString;
    float lightCmd = std::stof(lightCmdString);
    std::string ebrakeCmdString;
    m_replayFile >> ebrakeCmdString;
    float ebrakeCmd = std::stof(ebrakeCmdString);
    std::string brakeFLCmdString;
    m_replayFile >> brakeFLCmdString;
    float brakeFLCmd = std::stof(brakeFLCmdString);
    std::string brakeFRCmdString;
    m_replayFile >> brakeFRCmdString;
    float brakeFRCmd = std::stof(brakeFRCmdString);
    std::string brakeRLCmdString;
    m_replayFile >> brakeRLCmdString;
    float brakeRLCmd = std::stof(brakeRLCmdString);
    std::string brakeRRCmdString;
    m_replayFile >> brakeRRCmdString;
    float brakeRRCmd = std::stof(brakeRRCmdString);
    std::string wingFCmdString;
    m_replayFile >> wingFCmdString;
    float wingFCmd = std::stof(wingFCmdString);
    std::string wingRCmdString;
    m_replayFile >> wingRCmdString;
    float wingRCmd = std::stof(wingRCmdString);
    std::string telemetryModeString;
    m_replayFile >> telemetryModeString;
    float telemetryMode = std::stof(telemetryModeString);
    std::string singleWheelBrakeModeString;
    m_replayFile >> singleWheelBrakeModeString;
    float singleWheelBrakeMode = std::stof(singleWheelBrakeModeString);

    p_car->_accelCmd = accel;
    p_car->_brakeCmd = brake;
    p_car->_steerCmd = steer;
    p_car->_gearCmd = static_cast<int>(gear);
    p_car->_clutchCmd = clutch;
    p_car->_raceCmd = static_cast<int>(raceCmd);
    p_car->_lightCmd = static_cast<int>(lightCmd);
    p_car->_ebrakeCmd = static_cast<int>(ebrakeCmd);
    p_car->_brakeFLCmd = brakeFLCmd;
    p_car->_brakeFRCmd = brakeFRCmd;
    p_car->_brakeRLCmd = brakeRLCmd;
    p_car->_brakeRRCmd = brakeRRCmd;
    p_car->_wingFCmd = wingFCmd;
    p_car->_wingRCmd = wingRCmd;
    p_car->_telemetryMode = static_cast<int>(telemetryMode);
    p_car->_singleWheelBrakeMode = static_cast<int>(singleWheelBrakeMode);
    std::string inputTime;
    m_replayFile >> inputTime;

    if (m_replayFile.eof()) {
        p_situation->raceInfo.state = RM_RACE_ENDED;
    } else {
        m_inputTime = std::stod(inputTime);
    }

    SMediator::GetInstance()->DriveTick(p_car, p_situation);

}

/// @brief Pause the current race.
/// @param p_car The car the driver controls
/// @param p_situation The current race situation
void Driver::PauseRace(tCarElt* p_car, tSituation* p_situation) {
}

/// @brief Resume the current race.
/// @param p_car The car the driver controls
/// @param p_situation The current race situation
void Driver::ResumeRace(tCarElt* p_car, tSituation* p_situation) {
}

/// @brief Called when a pit stop starts
/// @param p_car The car the driver controls
/// @param p_situation The current race situation
/// @return The pit stop command either ROB_PIT_IM or ROB_PIT_MENU
int Driver::PitCmd(tCarElt* p_car, tSituation* p_situation) {
    return 0;
}

/// @brief End the current race.
/// @param p_car The car the driver controls
/// @param p_situation The current race situation
void Driver::EndRace(tCarElt* p_car, tSituation* p_situation) {
}

/// @brief Shutdown the driver.
/// Also tell the mediator the race has ended.
void Driver::Shutdown() {
    SMediator::GetInstance()->RaceStop();
}

/// @brief Terminate the driver.
void Driver::Terminate() {
}
