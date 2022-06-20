/*
 * This program has been developed by students from the bachelor Computer Science at
 * Utrecht University within the Software Project course.
 * © Copyright Utrecht University (Department of Information and Computing Sciences)
 */

#pragma once

#include <track.h>
#include <car.h>
#include <raceman.h>
#include <fstream>
#include <string>

class Driver
{
public:
    Driver();
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
    std::ifstream m_replayFile;
    std::ifstream m_recordedSimulationData;
    double m_inputTime;
};