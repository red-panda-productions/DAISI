#pragma once
#include "Mediator.h"

#define CREATE_MEDIATOR_IMPLEMENTATION(type)\
    template INTERVENTION_TYPE Mediator<type>::GetInterventionType(); \
	template void Mediator<type>::SetInterventionType(INTERVENTION_TYPE p_type);\
	template void Mediator<type>::DriveTick(tCarElt* p_car, tSituation* p_situation);\
    template void Mediator<type>::RaceStart(tTrack* p_track, void* p_carHandle, void** p_carParmHandle, tSituation* p_situation);\
	template void Mediator<type>::RaceStop();\
    template DriveSituation* Mediator<type>::Simulate();\
    template Mediator<type>& Mediator<type>::GetInstance(); \
	template Mediator<type>::Mediator();

template<typename DecisionMaker>
INTERVENTION_TYPE Mediator<DecisionMaker>::GetInterventionType()
{
    return m_decisionMaker.m_config.GetInterventionType();
}

template<typename DecisionMaker>
void Mediator<DecisionMaker>::SetInterventionType(INTERVENTION_TYPE p_type)
{
    m_decisionMaker.ChangeSettings(p_type);
}

template<typename DecisionMaker>
void Mediator<DecisionMaker>::DriveTick(tCarElt* p_car, tSituation* p_situation)
{
    DriveSituation currentSituation(
        EnvironmentInfo(0, 0, 0),
        CarInfo(
            TrackPosition(false, 0, 0, 0, 0),
            0, 0, 0, false),
        PlayerInfo(0, 0, 0, 0));

    m_decisionMaker.Decide(currentSituation);

}

template<typename DecisionMaker>
void Mediator<DecisionMaker>::RaceStart(tTrack* p_track, void* p_carHandle, void** p_carParmHandle, tSituation* p_situation) {}

template<typename DecisionMaker>
void Mediator<DecisionMaker>::RaceStop() {}

template<typename DecisionMaker>
DriveSituation* Mediator<DecisionMaker>::Simulate()
{
    return nullptr; 
}

template<typename DecisionMaker>
Mediator<DecisionMaker>& Mediator<DecisionMaker>::GetInstance() {
    static Mediator<DecisionMaker> s_instance;
    return s_instance;
}

template<typename DecisionMaker>
Mediator<DecisionMaker>::Mediator()
{

}