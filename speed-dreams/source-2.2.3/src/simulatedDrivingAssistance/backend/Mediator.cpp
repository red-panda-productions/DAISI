#include "Mediator.h"

static Mediator* s_instance = nullptr;

INTERVENTION_TYPE Mediator::GetInterventionType()
{
    return INTERVENTION_TYPE_NO_ASSISTANCE;
}

void Mediator::SetInterventionType(INTERVENTION_TYPE type)
{

}

void Mediator::DriveTick(tCarElt* p_car, tSituation* p_situation) {}

void Mediator::RaceStart(tTrack* p_track, void* p_carHandle, void** p_carParmHandle, tSituation* p_situation) {}

void Mediator::RaceStop(tCarElt* p_car, tSituation* p_situation) {}

DriveSituation* Mediator::Simulate()
{
    return nullptr;
}

Mediator* Mediator::GetInstance() {
    if(!s_instance) {
        s_instance = new Mediator();
    }

    return s_instance;
}
