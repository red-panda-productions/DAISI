#pragma once
#include "DriveSituation.h"
#include "ConfigEnums.h"
#include "SDAConfig.h"
#include "InterventionMaker.h"

class DecisionMaker
{
public:
    SDAConfig m_config;
    InterventionMaker* m_interventionMaker;

	/* TODO: Return [Command] */ void Decide(DriveSituation& driveSituation);

    // calls the config to return an intervention maker based on the p_type
    // and sets m_interventionMaker to return maker
	void ChangeSettings(INTERVENTION_TYPE p_type);
};