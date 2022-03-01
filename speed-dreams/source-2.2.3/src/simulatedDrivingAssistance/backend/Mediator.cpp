#include "Mediator.h"

INTERVENTION_TYPE Mediator::GetInterventionType()
{
    return INTERVENTION_TYPE_NO_ASSISTANCE;
}

void Mediator::SetInterventionType(INTERVENTION_TYPE type)
{

}

void Mediator::DriveTick() {}

void Mediator::RaceStart() {}

void Mediator::RaceStop() {}

DriveSituation* Mediator::Simulate()
{
    return nullptr;
}