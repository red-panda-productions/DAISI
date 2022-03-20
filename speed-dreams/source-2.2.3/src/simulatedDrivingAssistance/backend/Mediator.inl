#pragma once
#include "Mediator.h"
#include <fstream>
#include <portability.h>

#define CREATE_MEDIATOR_IMPLEMENTATION(type)\
    template InterventionType Mediator<type>::GetInterventionType(); \
	template void Mediator<type>::SetInterventionType(InterventionType p_type);\
	template void Mediator<type>::SetDataCollectionSettings(bool* p_dataSetting);\
	template void Mediator<type>::DriveTick(tCarElt* p_car, tSituation* p_situation);\
    template void Mediator<type>::RaceStart(tTrack* p_track, void* p_carHandle, void** p_carParmHandle, tSituation* p_situation);\
	template void Mediator<type>::RaceStop();\
    template DriveSituation* Mediator<type>::Simulate();\
    template Mediator<type>* Mediator<type>::GetInstance(); \
	template Mediator<type>::Mediator();

template<typename DecisionMaker>
InterventionType Mediator<DecisionMaker>::GetInterventionType()
{
    return m_decisionMaker.Config.GetInterventionType();
}

template<typename DecisionMaker>
void Mediator<DecisionMaker>::SetInterventionType(InterventionType p_type)
{
    m_decisionMaker.ChangeSettings(p_type);
}

template<typename DecisionMaker>
void Mediator<DecisionMaker>::SetDataCollectionSettings(bool* p_dataSetting)
{
    m_decisionMaker.SetDataCollectionSettings(p_dataSetting);
}

template<typename DecisionMaker>
void Mediator<DecisionMaker>::DriveTick(tCarElt* p_car, tSituation* p_situation)
{
    CarController.SetCar(p_car);
    DriveSituation currentSituation(
        m_environment,
        CarInfo(
            TrackPosition(false, p_car->pub.trkPos.toStart, p_car->pub.trkPos.toRight, p_car->pub.trkPos.toMiddle, p_car->pub.trkPos.toLeft),
            p_car->pub.DynGC.vel.x * 3.6f, p_car->race.topSpeed, p_car->priv.gear, false),
        PlayerInfo(p_car->ctrl.steer, p_car->ctrl.accelCmd, p_car->ctrl.brakeCmd, p_car->ctrl.clutchCmd));

    m_decisionMaker.Decide(currentSituation);
}

template<typename DecisionMaker>
void Mediator<DecisionMaker>::RaceStart(tTrack* p_track, void* p_carHandle, void** p_carParmHandle, tSituation* p_situation)
{
    m_environment.SetTimeOfDay(p_track->local.timeofday);
    m_environment.SetClouds(p_track->local.clouds);
    m_environment.SetRain(p_track->local.rain);

    DriveSituation currentSituation(
        m_environment,
        CarInfo(
            TrackPosition(false, 0, 0, 0, 0),
            0, 0, 0, false),
        PlayerInfo(0, 0, 0, 0));


    m_decisionMaker.Initialize(currentSituation);
}

template<typename DecisionMaker>
void Mediator<DecisionMaker>::RaceStop() {}

template<typename DecisionMaker>
DriveSituation* Mediator<DecisionMaker>::Simulate()
{
    return nullptr; 
}

template<typename DecisionMaker>
Mediator<DecisionMaker>* Mediator<DecisionMaker>::GetInstance() 
{
    if(m_instance == nullptr)
    {
        // check if Mediator file exists
        struct stat info;
        char workingDir[256];
        getcwd(workingDir,256);
        std::string workingDirectory(workingDir);
        workingDirectory += "\\Singletons\\Mediator";
        const char* filepath = workingDirectory.c_str();
        int err = stat(filepath, &info);
        if(err == -1)
        {
            // file does not exist create pointer
            m_instance = new Mediator();
            std::ofstream file("Singletons/Mediator");
            file << m_instance;
            file.close();
            return m_instance;
        }

        // file exists read pointer
        std::string pointerName("00000000");
        std::ifstream file("Singletons/Mediator");
        getline(file, pointerName);
        file.close();
        int pointerValue = stoi(pointerName, 0, 16);
        m_instance = (Mediator<DecisionMaker>*)pointerValue;
    }
    return m_instance;
}

template<typename DecisionMaker>
Mediator<DecisionMaker>::Mediator()
{

}
