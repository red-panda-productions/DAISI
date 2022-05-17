#include "Driver.h"
#include "Mediator.h"
#include "ConfigEnums.h"
#include <tgf.h>
#include <Recorder.h>

#include "../../simulatedDrivingAssistance/rppUtils/RppUtils.hpp"

namespace filesystem = std::experimental::filesystem;

/// @brief Initialize the driver with the given track
/// Make sure the human driver is initialized and ready to drive.
/// @param p_index The driver's index (starting from 1)
/// @param p_name The driver's name
Driver::Driver(int p_index, const char* p_name)
    : m_index(p_index), m_inputTime(0)
{
}

/// @brief Initialize the driver with the given track.
/// Also notify the mediator that the race starts.
/// @param p_track The track that is being initialized
/// @param p_carHandle
/// @param p_carParmHandle
/// @param p_situation The current race situation
void Driver::InitTrack(tTrack* p_track, void* p_carHandle, void** p_carParmHandle, tSituation* p_situation)
{
    const filesystem::path& replayFolder = SMediator::GetInstance()->GetReplayFolder();
    assert(Recorder::ValidateAndUpdateRecording(replayFolder));
    const filesystem::path carSettingsFile = filesystem::path(replayFolder).append(CAR_SETTINGS_FILE_NAME);
    const filesystem::path userRecordingFile = filesystem::path(replayFolder).append(USER_INPUT_RECORDING_FILE_NAME);
    const filesystem::path simulationFile = filesystem::path(replayFolder).append(SIMULATION_DATA_RECORDING_FILE_NAME);

    *p_carParmHandle =
        GfParmReadFile(carSettingsFile.string().c_str(),
                       GFPARM_RMODE_STD,
                       true);

    m_replayFile.open(userRecordingFile.string().c_str(), std::ios::binary);
    m_recordedSimulationData.open(simulationFile.string().c_str(), std::ios::binary);
    SMediator::GetInstance()->RaceStart(p_track, p_carHandle, p_carParmHandle, p_situation);
}

/// @brief Start a new race.
/// it opens the replay file and sets the time of the first input
/// @param p_car The car the driver controls
/// @param p_situation The current race situation
void Driver::NewRace(tCarElt* p_car, tSituation* p_situation)
{
    m_replayFile >> bits(m_inputTime);
}

/// @brief Validate whether the current simulation data matches the recorded simulation data.
/// @param p_car The current state of the car
/// @param p_simulationDataFile The recorded simulation data file
void ValidateSimulationData(tCarElt* p_car, std::ifstream& p_simulationDataFile)
{
    if (p_simulationDataFile.peek() == EOF) return;

    double currentTime;
    tPosd posG{};
    tPosd velG{};
    tPosd accG{};
    tPosd pos{};
    tPosd vel{};
    tPosd acc{};

    p_simulationDataFile >> bits(currentTime);
    p_simulationDataFile >> bits(posG.x);
    p_simulationDataFile >> bits(posG.y);
    p_simulationDataFile >> bits(posG.z);
    p_simulationDataFile >> bits(velG.x);
    p_simulationDataFile >> bits(velG.y);
    p_simulationDataFile >> bits(velG.z);
    p_simulationDataFile >> bits(accG.x);
    p_simulationDataFile >> bits(accG.y);
    p_simulationDataFile >> bits(accG.z);
    p_simulationDataFile >> bits(pos.x);
    p_simulationDataFile >> bits(pos.y);
    p_simulationDataFile >> bits(pos.z);
    p_simulationDataFile >> bits(vel.x);
    p_simulationDataFile >> bits(vel.y);
    p_simulationDataFile >> bits(vel.z);
    p_simulationDataFile >> bits(acc.x);
    p_simulationDataFile >> bits(acc.y);
    p_simulationDataFile >> bits(acc.z);

    assert(p_car->pub.DynGCg.pos.x == posG.x);
    assert(p_car->pub.DynGCg.pos.y == posG.y);
    assert(p_car->pub.DynGCg.pos.z == posG.z);
    assert(p_car->pub.DynGCg.vel.x == velG.x);
    assert(p_car->pub.DynGCg.vel.y == velG.y);
    assert(p_car->pub.DynGCg.vel.z == velG.z);
    assert(p_car->pub.DynGCg.acc.x == accG.x);
    assert(p_car->pub.DynGCg.acc.y == accG.y);
    assert(p_car->pub.DynGCg.acc.z == accG.z);
    assert(p_car->pub.DynGC.pos.x == pos.x);
    assert(p_car->pub.DynGC.pos.y == pos.y);
    assert(p_car->pub.DynGC.pos.z == pos.z);
    assert(p_car->pub.DynGC.vel.x == vel.x);
    assert(p_car->pub.DynGC.vel.y == vel.y);
    assert(p_car->pub.DynGC.vel.z == vel.z);
    assert(p_car->pub.DynGC.acc.x == acc.x);
    assert(p_car->pub.DynGC.acc.y == acc.y);
    assert(p_car->pub.DynGC.acc.z == acc.z);
}

/// @brief Update the car's controls based on recording at that currentTime.
/// It then gets input from the black-box, which changes the input as well
/// It ends the race if the file is done
/// @param p_car The car the driver controls
/// @param p_situation The current race situation
void Driver::Drive(tCarElt* p_car, tSituation* p_situation)
{
    ValidateSimulationData(p_car, m_recordedSimulationData);

    float accel;
    float brake;
    float steer;
    float gear;
    float clutch;

    m_replayFile >> bits(accel);
    m_replayFile >> bits(brake);
    m_replayFile >> bits(steer);
    m_replayFile >> bits(gear);
    m_replayFile >> bits(clutch);

    float raceCmd;
    float lightCmd;
    float ebrakeCmd;
    float brakeFLCmd;
    float brakeFRCmd;
    float brakeRLCmd;
    float brakeRRCmd;
    float wingFCmd;
    float wingRCmd;
    float telemetryMode;
    float singleWheelBrakeMode;

    m_replayFile >> bits(raceCmd);
    m_replayFile >> bits(lightCmd);
    m_replayFile >> bits(ebrakeCmd);
    m_replayFile >> bits(brakeFLCmd);
    m_replayFile >> bits(brakeFRCmd);
    m_replayFile >> bits(brakeRLCmd);
    m_replayFile >> bits(brakeRRCmd);
    m_replayFile >> bits(wingFCmd);
    m_replayFile >> bits(wingRCmd);
    m_replayFile >> bits(telemetryMode);
    m_replayFile >> bits(singleWheelBrakeMode);

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

    m_replayFile >> bits(m_inputTime);
    if (!m_replayFile)
    {
        p_situation->raceInfo.state = RM_RACE_ENDED;
    }

    SMediator::GetInstance()->DriveTick(p_car, p_situation);

    if (SMediator::GetInstance()->TimeOut())
    {
        p_situation->raceInfo.state = RM_RACE_ENDED;
    }
}

/// @brief Pause the current race.
/// @param p_car The car the driver controls
/// @param p_situation The current race situation
void Driver::PauseRace(tCarElt* p_car, tSituation* p_situation)
{
}

/// @brief Resume the current race.
/// @param p_car The car the driver controls
/// @param p_situation The current race situation
void Driver::ResumeRace(tCarElt* p_car, tSituation* p_situation)
{
}

/// @brief Called when a pit stop starts
/// @param p_car The car the driver controls
/// @param p_situation The current race situation
/// @return The pit stop command either ROB_PIT_IM or ROB_PIT_MENU
int Driver::PitCmd(tCarElt* p_car, tSituation* p_situation)
{
    return 0;
}

/// @brief End the current race.
/// @param p_car The car the driver controls
/// @param p_situation The current race situation
void Driver::EndRace(tCarElt* p_car, tSituation* p_situation)
{
}

/// @brief Shutdown the driver.
/// Also tell the mediator the race has ended.
void Driver::Shutdown()
{
    SMediator::GetInstance()->RaceStop();
}

/// @brief Terminate the driver.
void Driver::Terminate()
{
}
