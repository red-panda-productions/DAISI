#include "Driver.h"
#include "Mediator.h"
#include "ConfigEnums.h"

/// @brief Initialize the driver with the given track
/// Make sure the human driver is initialized and ready to drive.
/// @param p_index The driver's index (starting from 1)
/// @param p_name The driver's name
Driver::Driver(int p_index, const char* p_name) : m_index(p_index), m_humanDriver(p_name) {
    m_humanDriver.count_drivers();
    m_humanDriver.init_context(p_index);
    // Pretend like the module is just initializing
    auto* tempArr = new tModInfo[1];
    m_humanDriver.initialize(tempArr, nullptr);
    delete[] tempArr;
}

/// @brief Initialize the driver with the given track.
/// Also notify the mediator that the race starts.
/// @param p_track The track that is being initialized
/// @param p_carHandle
/// @param p_carParmHandle
/// @param p_situation The current race situation
void Driver::InitTrack(tTrack* p_track, void* p_carHandle, void** p_carParmHandle, tSituation* p_situation) {
    m_humanDriver.init_track(m_index, p_track, p_carHandle, p_carParmHandle, p_situation);

    Mediator::GetInstance().RaceStart(p_track, p_carHandle, p_carParmHandle, p_situation);
}

/// @brief Start a new race.
/// @param p_car The car the driver controls
/// @param p_situation The current race situation
void Driver::NewRace(tCarElt* p_car, tSituation* p_situation) {
    m_humanDriver.new_race(m_index, p_car, p_situation);
}

/// @brief Update the car's controls based on the current race situation.
/// In other words: Drive.
/// Ask the human driver for input and ask the mediator for controls.
/// @param p_car The car the driver controls
/// @param p_situation The current race situation
void Driver::Drive(tCarElt* p_car, tSituation* p_situation) {
    // Do not let the human control the car when the AI is in control
    if (Mediator::GetInstance().GetInterventionType() != INTERVENTION_TYPE_ALWAYS_INTERVENE) {
        m_humanDriver.drive_at(m_index, p_car, p_situation);
    }

    Mediator::GetInstance().DriveTick(p_car, p_situation);
}

/// @brief Pause the current race.
/// @param p_car The car the driver controls
/// @param p_situation The current race situation
void Driver::PauseRace(tCarElt* p_car, tSituation* p_situation) {
    m_humanDriver.pause_race(m_index, p_car, p_situation);
}

/// @brief Resume the current race.
/// @param p_car The car the driver controls
/// @param p_situation The current race situation
void Driver::ResumeRace(tCarElt* p_car, tSituation* p_situation) {
    m_humanDriver.resume_race(m_index, p_car, p_situation);
}

/// @brief Called when a pit stop starts
/// @param p_car The car the driver controls
/// @param p_situation The current race situation
/// @return The pit stop command either ROB_PIT_IM or ROB_PIT_MENU
int Driver::PitCmd(tCarElt* p_car, tSituation* p_situation) {
    return m_humanDriver.pit_cmd(m_index, p_car, p_situation);
}

/// @brief End the current race.
/// @param p_car The car the driver controls
/// @param p_situation The current race situation
void Driver::EndRace(tCarElt* p_car, tSituation* p_situation) {
    m_humanDriver.end_race(m_index, p_car, p_situation);
}

/// @brief Shutdown the driver.
/// Also tell the mediator the race has ended.
void Driver::Shutdown() {
    m_humanDriver.shutdown(m_index);
    Mediator::GetInstance().RaceStop();
}

/// @brief Terminate the driver.
void Driver::Terminate() {
    m_humanDriver.terminate();
}
