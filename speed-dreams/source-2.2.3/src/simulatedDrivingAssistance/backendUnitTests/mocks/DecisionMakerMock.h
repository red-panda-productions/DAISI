#pragma once
#include "DriveSituation.h"
#include "ConfigEnums.h"
class DecisionMakerMock
{
public:
	bool Decide(DriveSituation& driveSituation)
	{
		return Decision;
	}

	void ChangeSettings(INTERVENTION_TYPE p_type)
	{
		Type = p_type;
	}


	INTERVENTION_TYPE Type;
	bool Decision;
};