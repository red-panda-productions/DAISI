#pragma once
#include "DriveSituation.h"
#include "ConfigEnums.h"
#include "Config.h"
#include "InterventionMaker.h"
#include "SocketBlackBox.h"

template <typename BlackBox>
class DecisionMaker
{
public:
	/* TODO: Return [Command] */ bool Decide(DriveSituation& driveSituation);

    // calls the config to return an intervention maker based on the p_type
    // and sets m_interventionMaker to return maker
	void ChangeSettings(INTERVENTION_TYPE p_type);

    Config m_config;
    InterventionMaker* m_interventionMaker;
    BlackBox m_blackBox;
};

#define SDecisionMaker DecisionMaker<SSocketBlackBox> 