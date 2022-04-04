#pragma once
#include "ConfigEnums.h"

class DecisionMakerMock
{
public:
	bool Decide(tCarElt* p_car, tSituation* p_situation, int p_tickCount)
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