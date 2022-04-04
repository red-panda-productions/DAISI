#pragma once
#include "../../backend/DriveSituation.h"
#include "../../backend/ConfigEnums.h"
#include "../../backend/Recorder.h"

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

    ~DecisionMakerMock() {};
};