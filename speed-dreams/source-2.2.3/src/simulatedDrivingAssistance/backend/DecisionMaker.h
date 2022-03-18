#pragma once
#include "DriveSituation.h"
#include "ConfigEnums.h"
#include "SDAConfig.h"
#include "InterventionExecutor.h"
#include "SocketBlackBox.h"

/// @brief                 A class that can ask the black box to make a decision
/// @tparam SocketBlackBox The SocketBlackBox type
/// @tparam SDAConfig      The config type
template <typename SocketBlackBox, typename SDAConfig>
class DecisionMaker
{
public:
    SDAConfig Config;

	bool Decide(DriveSituation& driveSituation);

	void ChangeSettings(INTERVENTION_TYPE p_type);
    void SetDataCollectionSettings(bool* p_dataSetting);

    InterventionExecutor* m_interventionExecutor;
    SocketBlackBox m_blackBox;
};

/// @brief The standard type of the decisionMaker
#define SDecisionMaker DecisionMaker<SSocketBlackBox,SDAConfig>