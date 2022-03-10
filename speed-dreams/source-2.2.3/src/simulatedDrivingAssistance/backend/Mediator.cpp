#include "Mediator.h"


INTERVENTION_TYPE Mediator::GetInterventionType()
{
    return m_decisionMaker.m_config.GetInterventionType();
}

void Mediator::SetInterventionType(INTERVENTION_TYPE p_type)
{
    m_decisionMaker.ChangeSettings(p_type);
}

void Mediator::DriveTick() {}

void Mediator::RaceStart() {}

void Mediator::RaceStop() {}

DriveSituation* Mediator::Simulate()
{
    return nullptr;
}