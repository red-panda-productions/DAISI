#pragma once
#include "DriveSituation.h"
#include "ConfigEnums.h"
#include "Config.h"
#include "InterventionExecutor.h"
#include "SocketBlackBox.h"

/// @brief                 A class that can ask the black box to make a decision
/// @tparam SocketBlackBox The SocketBlackBox type
/// @tparam Config         The config type
template <typename SocketBlackBox, typename Config>
class DecisionMaker
{
public:
	bool Decide(DriveSituation& driveSituation);

	void ChangeSettings(INTERVENTION_TYPE p_type);

    Config m_config;
    InterventionExecutor* m_interventionExecutor;
    SocketBlackBox m_blackBox;
};

/// @brief The standard type of the decisionMaker
#define SDecisionMaker DecisionMaker<SSocketBlackBox,Config> 