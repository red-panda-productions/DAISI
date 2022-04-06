#include "Driver.h"
#include "Mediator.h"
#include "ConfigEnums.h"
#include <tgf.h>
#include <Recorder.h>
Recorder* recorder;
#define PARAMAMOUNT 16

const float Driver::SHIFT = 0.9;         /* [-] (% of rpmredline) */
const float Driver::SHIFT_MARGIN = 4.0;  /* [m/s] */

/// @brief Initialize the driver with the given track
/// Make sure the human driver is initialized and ready to drive.
/// @param p_index The driver's index (starting from 1)
/// @param p_name The driver's name
Driver::Driver(int p_index, const char* p_name) : m_index(p_index) {
    //m_humanDriver.count_drivers();
    //m_humanDriver.init_context(p_index);
    // Pretend like the module is just initializing
    auto* tempArr = new tModInfo[1];
    //m_humanDriver.initialize(tempArr, nullptr);
    delete[] tempArr;
}

/// @brief Initialize the driver with the given track.
/// Also notify the mediator that the race starts.
/// @param p_track The track that is being initialized
/// @param p_carHandle
/// @param p_carParmHandle
/// @param p_situation The current race situation
void Driver::InitTrack(tTrack* p_track, void* p_carHandle, void** p_carParmHandle, tSituation* p_situation) {
    //m_humanDriver.init_track(m_index, p_track, p_carHandle, p_carParmHandle, p_situation);

    //SMediator::GetInstance()->RaceStart(p_track, p_carHandle, p_carParmHandle, p_situation);
}

/// @brief Start a new race.
/// it opens te replay file and sets the time of the first input
/// @param p_car The car the driver controls
/// @param p_situation The current race situation
void Driver::NewRace(tCarElt* p_car, tSituation* p_situation) {
    recorder = new Recorder("user_recordings", "userRecording", PARAMAMOUNT);

    //m_humanDriver.new_race(m_index, p_car, p_situation);
    m_replayFile.open("..\\test_data\\user_recordings\\userRecording20220406-162354.txt");
    std::string inputTime;
    m_replayFile >> inputTime;
    m_inputTime = std::stod(inputTime);
}

/// @brief Update the car's controls based on recording at that currentTime.
/// It then gets input from the black-box, which changes the input as well
/// It ends the race if the file is done
/// @param p_car The car the driver controls
/// @param p_situation The current race situation
void Driver::Drive(tCarElt* p_car, tSituation* p_situation)
{
    //p_car->ctrl.gear = 1;//getGear(p_car);
    // input time isn't as accurate as currentTime,
    // so some margin needs to be added
    // Does not appear to lead to input inaccuracies
    // since this function is called around the same time
    // it is called in the recording
    //if(m_inputTime - 0.0001 <=  p_situation -> currentTime
    //    && p_situation->currentTime <= m_inputTime + 0.0001)
    {
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
        p_car->_gearCmd = gear;
        p_car->_clutchCmd = clutch;
        p_car->_raceCmd = raceCmd;
        p_car->_lightCmd = lightCmd;
        p_car->_ebrakeCmd = ebrakeCmd;
        p_car->_brakeFLCmd = brakeFLCmd;
        p_car->_brakeFRCmd = brakeFRCmd;
        p_car->_brakeRLCmd = brakeRLCmd;
        p_car->_brakeRRCmd = brakeRRCmd;
        p_car->_wingFCmd = wingFCmd;
        p_car->_wingRCmd = wingRCmd;
        p_car->_telemetryMode = telemetryMode;
        p_car->_singleWheelBrakeMode = singleWheelBrakeMode;
        std::string inputTime;
        m_replayFile >> inputTime;
        if (m_replayFile.eof())
        {
        	p_situation->raceInfo.state = RM_RACE_ENDED;
        }
        else
			m_inputTime = std::stod(inputTime);

        float inputs[PARAMAMOUNT] = {
            p_car->_accelCmd ,
            p_car->_brakeCmd,
            p_car->_steerCmd,
            p_car -> _gearCmd,
            p_car -> _clutchCmd,
            p_car->_raceCmd,
            p_car->_lightCmd,
            p_car->_ebrakeCmd,
            p_car->_brakeFLCmd,
            p_car->_brakeFRCmd,
            p_car->_brakeRLCmd,
            p_car->_brakeRRCmd,
            p_car->_wingFCmd,
            p_car->_wingRCmd,
            p_car->_telemetryMode,
            p_car->_singleWheelBrakeMode};
        recorder->WriteRecording(inputs, p_situation->currentTime, false);
    }
    //SMediator::GetInstance()->DriveTick(p_car, p_situation);

}

/* Compute gear */
int Driver::getGear(tCarElt* car)
{
    if (car->_gear <= 0) return 1;

    float gr_up = car->_gearRatio[car->_gear + car->_gearOffset];
    float omega = car->_enginerpmRedLine / gr_up;
    float wr = car->_wheelRadius(2);

    if (omega * wr * SHIFT < car->_speed_x) 
    {
        return car->_gear + 1;
    }
    else 
    {
        float gr_down = car->_gearRatio[car->_gear + car->_gearOffset - 1];
        omega = car->_enginerpmRedLine / gr_down;
        if (car->_gear > 1 && omega * wr * SHIFT > car->_speed_x + SHIFT_MARGIN) {
            return car->_gear - 1;
        }
    }
    return car->_gear;
}



/// @brief Pause the current race.
/// @param p_car The car the driver controls
/// @param p_situation The current race situation
void Driver::PauseRace(tCarElt* p_car, tSituation* p_situation) {
    //m_humanDriver.pause_race(m_index, p_car, p_situation);
}

/// @brief Resume the current race.
/// @param p_car The car the driver controls
/// @param p_situation The current race situation
void Driver::ResumeRace(tCarElt* p_car, tSituation* p_situation) {
    //m_humanDriver.resume_race(m_index, p_car, p_situation);
}

/// @brief Called when a pit stop starts
/// @param p_car The car the driver controls
/// @param p_situation The current race situation
/// @return The pit stop command either ROB_PIT_IM or ROB_PIT_MENU
int Driver::PitCmd(tCarElt* p_car, tSituation* p_situation) {
    return 0; //m_humanDriver.pit_cmd(m_index, p_car, p_situation);
}

/// @brief End the current race.
/// @param p_car The car the driver controls
/// @param p_situation The current race situation
void Driver::EndRace(tCarElt* p_car, tSituation* p_situation) {
    //m_humanDriver.end_race(m_index, p_car, p_situation);
}

/// @brief Shutdown the driver.
/// Also tell the mediator the race has ended.
void Driver::Shutdown() {
    //m_humanDriver.shutdown(m_index);
    SMediator::GetInstance()->RaceStop();
}

/// @brief Terminate the driver.
void Driver::Terminate() {
    //m_humanDriver.terminate();
}
