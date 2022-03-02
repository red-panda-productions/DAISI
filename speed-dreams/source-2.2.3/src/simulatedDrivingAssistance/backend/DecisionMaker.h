#pragma once
#include "DriveSituation.h"
#include "InterventionType.h"
#include "ConfigEnums.h"
#include "Config.h"
#include "IInterventionMaker.h"

class DecisionMaker
{
public:
    Config m_config;
    IInterventionMaker* m_interventionMaker;

	/* TODO: Return [Command] */ void Decide(DriveSituation& driveSituation);
	void ChangeSettings(INTERVENTION_TYPE type);
};