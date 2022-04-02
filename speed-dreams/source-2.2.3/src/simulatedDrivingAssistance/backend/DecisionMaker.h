#pragma once
#include "ConfigEnums.h"
#include "SDAConfig.h"
#include "InterventionExecutor.h"
#include "SocketBlackBox.h"
#include "BlackBoxData.h"

/// @brief                 A class that can ask the black box to make a decision
/// @tparam SocketBlackBox The SocketBlackBox type
/// @tparam SDAConfig      The config type
template <typename SocketBlackBox, typename SDAConfig>
class DecisionMaker
{
public:
    SDAConfig Config;

    void Initialize(tCarElt* p_initialCar, tSituation* p_initialSituation, BlackBoxData* p_testSituations = nullptr, int p_testAmount = 0);

    bool Decide(tCarElt* p_car, tSituation* p_situation, int p_tickCount);

    void ChangeSettings(InterventionType p_type);
    void SetDataCollectionSettings(tDataToStore p_dataSetting);

    InterventionExecutor* m_interventionExecutor;
    SocketBlackBox m_blackBox;
};

/// @brief The standard type of the decisionMaker
#define SDecisionMaker DecisionMaker<SSocketBlackBox,SDAConfig>