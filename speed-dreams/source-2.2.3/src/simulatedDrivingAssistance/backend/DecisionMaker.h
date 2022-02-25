#pragma once
#include "DriveSituation.h"
#include "InterventionType.h"

class DecisionMaker
{
public:
	/* TODO: Return [Command] */ void Decide(DriveSituation& driveSituation);
	void ChangeSettings(INTERVENTION_TYPE type);
};