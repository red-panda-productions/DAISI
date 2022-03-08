#pragma once
#include "DriveSituation.h"
#include "InterventionType.h"
#include "DecisionMaker.h"
#include "car.h"
#include "raceman.h"

class Mediator
{
public:
	void DriveTick(tCarElt* p_car, tSituation* p_situation);
	void RaceStart(tTrack* p_track, void* p_carHandle, void** p_carParmHandle, tSituation* p_situation);
	void RaceStop(tCarElt* p_car, tSituation* p_situation);
	void SetInterventionType(INTERVENTION_TYPE type);
    INTERVENTION_TYPE GetInterventionType();
	DriveSituation* Simulate();
    static Mediator* GetInstance();

 private:
	DriveSituation m_situation;
	DecisionMaker m_decisionMaker;

};