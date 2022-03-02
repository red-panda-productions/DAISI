#pragma once
#include "DriveSituation.h"
#include "ConfigEnums.h"
#include "Config.h"

class DecisionMaker
{
public:
    Config m_config;

	/* TODO: Return [Command] */ void Decide(DriveSituation& driveSituation);
	void ChangeSettings(INTERVENTION_TYPE type);
};