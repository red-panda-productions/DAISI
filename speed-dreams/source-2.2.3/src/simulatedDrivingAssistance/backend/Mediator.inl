#pragma once
#include "Mediator.h"

#define CREATE_MEDIATOR_IMPLEMENTATION(type)\
    template InterventionType Mediator<type>::GetInterventionType(); \
	template void Mediator<type>::SetTask(Task p_task);\
	template void Mediator<type>::SetIndicatorSettings(bool* p_indicators);\
	template void Mediator<type>::SetInterventionType(InterventionType p_type);\
	template void Mediator<type>::SetMaxTime(int p_maxTime);\
	template void Mediator<type>::SetUserID(char* p_userID);\
	template void Mediator<type>::SetDataCollectionSettings(bool* p_dataSetting);\
	template void Mediator<type>::DriveTick(tCarElt* p_car, tSituation* p_situation);\
    template void Mediator<type>::RaceStart(tTrack* p_track, void* p_carHandle, void** p_carParmHandle, tSituation* p_situation);\
	template void Mediator<type>::RaceStop();\
    template DriveSituation* Mediator<type>::Simulate();\
    template Mediator<type>& Mediator<type>::GetInstance(); \
	template Mediator<type>::Mediator();

template<typename DecisionMaker>
void Mediator<DecisionMaker>::SetTask(Task p_task) {
    m_decisionMaker.Config.SetTask(p_task);
}

template<typename DecisionMaker>
void Mediator<DecisionMaker>::SetIndicatorSettings(bool* p_indicators) {
    m_decisionMaker.Config.SetIndicatorSettings(p_indicators);
}

template<typename DecisionMaker>
InterventionType Mediator<DecisionMaker>::GetInterventionType(){
    return m_decisionMaker.Config.GetInterventionType();
}

template<typename DecisionMaker>
void Mediator<DecisionMaker>::SetInterventionType(InterventionType p_type){
    m_decisionMaker.ChangeSettings(p_type);
}

template<typename DecisionMaker>
void Mediator<DecisionMaker>::SetMaxTime(int p_maxTime) {
    m_decisionMaker.Config.SetMaxTime(p_maxTime);
}

template<typename DecisionMaker>
void Mediator<DecisionMaker>::SetUserID(char* p_userID) {
    m_decisionMaker.Config.SetUserID(p_userID);
}

template<typename DecisionMaker>
void Mediator<DecisionMaker>::SetDataCollectionSettings(bool* p_dataSetting){
    m_decisionMaker.SetDataCollectionSettings(p_dataSetting);
}

template<typename DecisionMaker>
void Mediator<DecisionMaker>::DriveTick(tCarElt* p_car, tSituation* p_situation) {}

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