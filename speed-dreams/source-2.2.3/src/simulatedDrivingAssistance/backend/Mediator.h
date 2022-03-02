#pragma once
#include "DriveSituation.h"
#include "ConfigEnums.h"
#include "DecisionMaker.h"

class Mediator
{
public:
	void DriveTick();
	void RaceStart();
	void RaceStop();
	void SetInterventionType(INTERVENTION_TYPE p_type);
    INTERVENTION_TYPE GetInterventionType();
	DriveSituation* Simulate();

private:
	DriveSituation m_situation;
	DecisionMaker m_decisionMaker;

};