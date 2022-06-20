/*
 * This program has been developed by students from the bachelor Computer Science at
 * Utrecht University within the Software Project course.
 * © Copyright Utrecht University (Department of Information and Computing Sciences)
 */

#pragma once

#include <track.h>
#include <car.h>
#include <raceman.h>
#include <humandriver.h>
#include "Recorder.h"

class Driver
{
public:
    Driver(int p_index, const char* p_name);
    void InitTrack(tTrack* p_track, void* p_carHandle, void** p_carParmHandle, tSituation* p_situation);
    void NewRace(tCarElt* p_car, tSituation* p_situation);
    void Drive(tCarElt* p_car, tSituation* p_situation);
    void PauseRace(tCarElt* p_car, tSituation* p_situation);
    void ResumeRace(tCarElt* p_car, tSituation* p_situation);
    int PitCmd(tCarElt* p_car, tSituation* p_situation);
    void EndRace(tCarElt* p_car, tSituation* p_situation);
    void Shutdown();
    void Terminate();

private:
    // The driver's index
    int m_index;
    // The human driver controller
    HumanDriver m_humanDriver;
    // The recorder
    Recorder* m_recorder;
};