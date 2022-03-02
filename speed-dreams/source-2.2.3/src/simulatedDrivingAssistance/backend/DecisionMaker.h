#pragma once
#include "DriveSituation.h"
#include "ConfigEnums.h"
#include "Config.h"
#include "InterventionMaker.h"

class DecisionMaker
{
public:
    Config m_config;
    InterventionMaker* m_interventionMaker;

	/* TODO: Return [Command] */ void Decide(DriveSituation& driveSituation);
	void ChangeSettings(INTERVENTION_TYPE type);
};