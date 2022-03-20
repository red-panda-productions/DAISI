#pragma once
#include "DecisionMaker.h"
#include "DecisionTuple.h"
#include "Mediator.h"

#define CREATE_DECISION_MAKER_IMPLEMENTATION(type1,type2) \
    template void DecisionMaker<type1,type2>::Initialize(DriveSituation& p_initialSituation,DriveSituation* p_testSituations, int p_testAmount);\
    template bool DecisionMaker<type1,type2>::Decide(DriveSituation& p_driveSituation);\
    template void DecisionMaker<type1,type2>::ChangeSettings(InterventionType p_type);\
    template void DecisionMaker<type1,type2>::SetDataCollectionSettings(bool* p_dataSetting);

#define TEMP_DECISIONMAKER DecisionMaker<SocketBlackBox,SDAConfig>

template <typename SocketBlackBox, typename SDAConfig>
void DecisionMaker<SocketBlackBox, SDAConfig>::Initialize(DriveSituation& p_initialSituation,
	DriveSituation* p_testSituations, int p_testAmount)
{
    m_blackBox.Initialize(p_initialSituation, p_testSituations, p_testAmount);
}

/// @brief                   Tries to get a decision from the black box
/// @param  p_driveSituation The current drive situation
/// @return                  Whether a decision was made
template <typename SocketBlackBox,typename SDAConfig>
bool TEMP_DECISIONMAKER::Decide(DriveSituation& p_driveSituation)
{
    DecisionTuple decision;
    if (!m_blackBox.GetDecisions(p_driveSituation, decision)) return false;

    Mediator<DecisionMaker<SocketBlackBox, SDAConfig>>::GetInstance()->CarController.ShowUI(INTERVENTION_ACTION_NONE);
    int decisionCount = 0;
    IDecision** decisions = decision.GetActiveDecisions(decisionCount);
    m_interventionExecutor->RunDecision(decisions, decisionCount);
    return true;
}

/// @brief         Changes the settings of how decisions should be made
/// @param  p_type The new type of interventions
template<typename SocketBlackBox,typename SDAConfig>
void TEMP_DECISIONMAKER::ChangeSettings(InterventionType p_type)
{
    m_interventionExecutor = Config.SetInterventionType(p_type);
}

/// @brief         Changes the settings of how decisions should be made
/// @param  p_type The new type of interventions
template<typename SocketBlackBox,typename SDAConfig>
void TEMP_DECISIONMAKER::SetDataCollectionSettings(bool* p_dataSetting)
{
    Config.SetDataCollectionSettings(p_dataSetting);
}