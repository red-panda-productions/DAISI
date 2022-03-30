#pragma once
#include "DriveSituation.h"
#include "ConfigEnums.h"
#include "SDAConfig.h"
#include "InterventionExecutor.h"
#include "SocketBlackBox.h"
#include "Recorder.h"
#include "ConfigEnums.h"

/// @brief                 A class that can ask the black box to make a decision
/// @tparam SocketBlackBox The SocketBlackBox type
/// @tparam SDAConfig      The config type
template <typename SocketBlackBox, typename SDAConfig>
class DecisionMaker
{
public:
    SDAConfig Config;

    void Initialize(DriveSituation& p_initialSituation, DriveSituation* p_testSituations = nullptr, int p_testAmount = 0);

    bool Decide(DriveSituation& p_driveSituation);

    void ChangeSettings(InterventionType p_type);
    void SetDataCollectionSettings(bool* p_dataSetting);

    InterventionExecutor* InterventionExecutor;
    SocketBlackBox BlackBox;

    ~DecisionMaker();

private:
#ifdef BB_RECORD_SESSION
    Recorder* m_recorder;
#endif
};


/// @brief The standard type of the decisionMaker
#define SDecisionMaker DecisionMaker<SSocketBlackBox,SDAConfig>