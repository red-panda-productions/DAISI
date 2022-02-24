#pragma once
#include "DriveSituation.h"
#include "InterventionType.h"
class DecisionMaker
{
public:
	Decide(DriveSituation& driveSituation);
	ChangeSettings(INTERVENTION_TYPE type);
};