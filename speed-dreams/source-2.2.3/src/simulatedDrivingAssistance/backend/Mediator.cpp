#include "Mediator.h"

static Mediator* s_instance = nullptr;\

INTERVENTION_TYPE Mediator::GetInterventionType()
{
    return m_decisionMaker.m_config.GetInterventionType();
}

void Mediator::SetInterventionType(INTERVENTION_TYPE p_type)
{
    m_decisionMaker.ChangeSettings(p_type);
}

/// @brief Sets the settings for data collection
/// @param p_dataSetting An array of booleans to enable/disable the collection of simulation data for research
void Mediator::SetDataCollectionSettings(bool p_dataSetting[])
{
    m_decisionMaker.m_config.SetDataCollectionSetting(p_dataSetting);
}

void Mediator::DriveTick(tCarElt* p_car, tSituation* p_situation) {}

void Mediator::RaceStart(tTrack* p_track, void* p_carHandle, void** p_carParmHandle, tSituation* p_situation) {}

void Mediator::RaceStop() {}

DriveSituation* Mediator::Simulate()
{
    return nullptr;
}

Mediator& Mediator::GetInstance() {
    static Mediator s_instance;
    return s_instance;
}

Mediator::Mediator()
{
}
