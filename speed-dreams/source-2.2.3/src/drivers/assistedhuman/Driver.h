#pragma once

#include <track.h>
#include <car.h>
#include <raceman.h>
#include <humandriver.h>
#include "Mediator.h"

class Driver {
 public:
    /// @brief Initialize the driver with the given track 
    /// @param p_index The driver's index (starting from 1)
    /// @param p_name The driver's name
    Driver(int p_index, const char* p_name);

    /// @brief Initialize the driver with the given track
    /// @param p_track The track that is being initialized
    /// @param p_carHandle 
    /// @param p_carParmHandle 
    /// @param p_situation The current race situation
    void InitTrack(tTrack* p_track, void* p_carHandle, void** p_carParmHandle, tSituation* p_situation);

    /// @brief Start a new race.
    /// @param p_car The car the driver controls
    /// @param p_situation The current race situation
    void NewRace(tCarElt* p_car, tSituation* p_situation);

    /// @brief Update the car's controls based on the current race situation.
    /// In other words: Drive.
    /// @param p_car The car the driver controls
    /// @param p_situation The current race situation
    void Drive(tCarElt* p_car, tSituation* p_situation);

    /// @brief Pause the current race.
    /// @param p_car The car the driver controls
    /// @param p_situation The current race situation
    void PauseRace(tCarElt* p_car, tSituation* p_situation);

    /// @brief Resume the current race.
    /// @param p_car The car the driver controls
    /// @param p_situation The current race situation
    void ResumeRace(tCarElt* p_car, tSituation* p_situation);

    int PitCmd(tCarElt* p_car, tSituation* p_situation);

    /// @brief End the current race.
    /// @param p_car The car the driver controls
    /// @param p_situation The current race situation
    void EndRace(tCarElt* p_car, tSituation* p_situation);

    /// @brief Shutdown the driver.
    void Shutdown();

    /// @brief Terminate the driver.
    void Terminate();

 private:
    // The driver's index
    int m_index;
    // The human driver controller
    HumanDriver m_humanDriver;
};