#pragma once

#include <track.h>
#include <car.h>
#include <raceman.h>
#include <humandriver.h>

class Driver {
 public:
    /// <summary>
    /// Initialize the driver with the given track
    /// </summary>
    /// <param name="p_index">The driver's index (starting from 1)</param>
    /// <param name="p_name">The driver's name</param>
    Driver(int p_index, const char* p_name);

    /// <summary>
    /// Initialize the driver with the given track
    /// </summary>
    /// <param name="p_track">The track that is being initialized</param>
    /// <param name="p_carHandle"></param>
    /// <param name="p_carParmHandle"></param>
    /// <param name="p_situation">The current race situation</param>
    void InitTrack(tTrack* p_track, void* p_carHandle, void** p_carParmHandle, tSituation* p_situation);

    /// <summary>
    /// Start a new race.
    /// </summary>
    /// <param name="p_car">The car the driver controls</param>
    /// <param name="p_situation">The current race situation</param>
    void NewRace(tCarElt* p_car, tSituation* p_situation);

    /// <summary>
    /// Update the car's controls based on the current race situation.
    /// In other words: Drive.
    /// </summary>
    /// <param name="p_car">The car the driver controls</param>
    /// <param name="p_situation">The current race situation</param>
    void Drive(tCarElt* p_car, tSituation* p_situation);

    /// <summary>
    /// Pause the current race.
    /// </summary>
    /// <param name="p_car">The car the driver controls</param>
    /// <param name="p_situation">The current race situation</param>
    void PauseRace(tCarElt* p_car, tSituation* p_situation);

    /// <summary>
    /// Resume the current race.
    /// </summary>
    /// <param name="p_car">The car the driver controls</param>
    /// <param name="p_situation">The current race situation</param>
    void ResumeRace(tCarElt* p_car, tSituation* p_situation);

    int PitCmd(tCarElt* p_car, tSituation* p_situation);

    /// <summary>
    /// End the current race.
    /// </summary>
    /// <param name="p_car">The car the driver controls</param>
    /// <param name="p_situation">The current race situation</param>
    void EndRace(tCarElt* p_car, tSituation* p_situation);

    /// <summary>
    /// Shutdown the driver.
    /// </summary>
    void Shutdown();

    /// <summary>
    /// Terminate the driver.
    /// </summary>
    void Terminate();

 private:
    // The driver's index
    int m_index;
    // The human driver controller
    HumanDriver m_humanDriver;
};