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

    // calls decision maker to change its intervention setting to p_type
	void SetInterventionType(INTERVENTION_TYPE p_type);

    //returns the intervention type from the decision maker
    INTERVENTION_TYPE GetInterventionType();
	DriveSituation* Simulate();

private:
	DriveSituation m_situation;
	DecisionMaker m_decisionMaker;

};