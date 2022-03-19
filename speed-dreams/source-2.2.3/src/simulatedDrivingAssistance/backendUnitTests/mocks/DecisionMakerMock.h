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

	void ChangeSettings(InterventionType p_type)
	{
		Type = p_type;
	}

    InterventionType Type;
	bool Decision;
};