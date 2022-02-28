#pragma once
#include "DriveSituation.h"
#include "InterventionType.h"
#include "DecisionMaker.h"

class Mediator
{
public:
	void DriveTick();
	void RaceStart();
	void RaceStop();
	void SetInterventionType(INTERVENTION_TYPE type);
    INTERVENTION_TYPE GetInterventionType();
	DriveSituation* Simulate();

private:
	DriveSituation m_situation;
	DecisionMaker m_decisionMaker;

};