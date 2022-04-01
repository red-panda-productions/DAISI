#pragma once
#include "Mediator.h"
#include <fstream>
#include <portability.h>
#include <SDL2/SDL_main.h>

/// @brief Creates an implementation of the mediator
#define CREATE_MEDIATOR_IMPLEMENTATION(type)\
    template InterventionType Mediator<type>::GetInterventionType(); \
    template tIndicator Mediator<type>::GetIndicatorSettings(); \
	template void Mediator<type>::SetTask(Task p_task);\
	template void Mediator<type>::SetIndicatorSettings(tIndicator p_indicators);\
	template void Mediator<type>::SetInterventionType(InterventionType p_type);\
	template void Mediator<type>::SetMaxTime(int p_maxTime);\
	template void Mediator<type>::SetUserId(char* p_userId);\
	template void Mediator<type>::SetDataCollectionSettings(tDataToStore p_dataSetting);\
	template void Mediator<type>::DriveTick(tCarElt* p_car, tSituation* p_situation);\
    template void Mediator<type>::RaceStart(tTrack* p_track, void* p_carHandle, void** p_carParmHandle, tSituation* p_situation);\
	template void Mediator<type>::RaceStop();\
    template Mediator<type>* Mediator<type>::GetInstance(); \
	template Mediator<type>::Mediator();


/// @brief        Sets the task in SDAConfig to p_task
/// @param p_task The Task
template<typename DecisionMaker>
void Mediator<DecisionMaker>::SetTask(Task p_task)
{
    m_decisionMaker.Config.SetTask(p_task);
}

/// @brief              Sets the settings for indication of interventions
/// @param p_indicators The Indicator settings
template<typename DecisionMaker>
void Mediator<DecisionMaker>::SetIndicatorSettings(tIndicator p_indicators)
{
    m_decisionMaker.Config.SetIndicatorSettings(p_indicators);
}

/// @brief  Gets the intervention type from the decision maker
/// @return The intervention type from the decision maker
template<typename DecisionMaker>
InterventionType Mediator<DecisionMaker>::GetInterventionType()
{
    return m_decisionMaker.Config.GetInterventionType();
}

/// @brief        Sets the settings for interventionType to p_type
/// @param p_type The InterventionType
template<typename DecisionMaker>
void Mediator<DecisionMaker>::SetInterventionType(InterventionType p_type)
{
    m_decisionMaker.ChangeSettings(p_type);
}

/// @brief           Sets the maximum simulation time to p_maxTime
/// @param p_maxTime The maximum simulation time
template<typename DecisionMaker>
void Mediator<DecisionMaker>::SetMaxTime(int p_maxTime)
{
    m_decisionMaker.Config.SetMaxTime(p_maxTime);
}

/// @brief          Sets the userID to p_userID
/// @param p_userID The userID
template<typename DecisionMaker>
void Mediator<DecisionMaker>::SetUserId(char* p_userId)
{
    m_decisionMaker.Config.SetUserId(p_userId);
}

/// @brief               Sets the settings for data collection
/// @param p_dataSetting An array of booleans to enable/disable the collection of simulation data for research
template<typename DecisionMaker>
void Mediator<DecisionMaker>::SetDataCollectionSettings(tDataToStore p_dataSetting)
{
    m_decisionMaker.SetDataCollectionSettings(p_dataSetting);
}

/// @brief             Gets the setting for the given indicator
/// @param p_indicator Indicator whose setting to get
/// @return true if the indicator is enabled, false when disabled
template <typename DecisionMaker>
tIndicator Mediator<DecisionMaker>::GetIndicatorSettings()
{
    return m_decisionMaker.Config.GetIndicatorSettings();
}

template<typename DecisionMaker>
void Mediator<DecisionMaker>::DriveTick(tCarElt* p_car, tSituation* p_situation)
{
    m_tickCount++;
    CarController.SetCar(p_car);
    m_decisionMaker.Decide(p_car, p_situation,m_tickCount);
}

template<typename DecisionMaker>
void Mediator<DecisionMaker>::RaceStart(tTrack* p_track, void* p_carHandle, void** p_carParmHandle, tSituation* p_situation)
{
    m_track = p_track;
    tCarElt car;
    m_decisionMaker.Initialize(&car, p_situation);
}

template<typename DecisionMaker>
void Mediator<DecisionMaker>::RaceStop() {}


/// @brief Creates a mediator instance if needed and returns it
/// @return A mediator instance
template<typename DecisionMaker>
Mediator<DecisionMaker>* Mediator<DecisionMaker>::GetInstance()
{
    if (m_instance == nullptr)
    {
        // check if Mediator file exists
        struct stat info;
        char workingDir[256];
        getcwd(workingDir, 256);
        std::string workingDirectory(workingDir);
        workingDirectory += "\\Singletons\\Mediator";
        const char* filepath = workingDirectory.c_str();
        int err = stat(filepath, &info);
        if (err == -1)
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