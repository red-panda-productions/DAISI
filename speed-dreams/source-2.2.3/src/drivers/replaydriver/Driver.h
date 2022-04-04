#pragma once

#include <track.h>
#include <car.h>
#include <raceman.h>
#include <fstream>
#include <string>


class Driver {
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
    std::ifstream m_replayFile;
	double m_inputTime;

};