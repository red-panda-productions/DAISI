#pragma once
#include "DecisionMaker.h"
#include "DecisionTuple.h"

#define CREATE_DECISION_MAKER_IMPLEMENTATION(type1,type2) \
    template bool DecisionMaker<type1,type2>::Decide(DriveSituation& p_driveSituation);\
    template void DecisionMaker<type1,type2>::ChangeSettings(INTERVENTION_TYPE p_type);\
    template void DecisionMaker<type1,type2>::SetDataCollectionSettings(bool* p_dataSetting);

#define TEMP_DECISIONMAKER DecisionMaker<SocketBlackBox,SDAConfig>

/// @brief                   Tries to get a decision from the black box
/// @param  p_driveSituation The current drive situation
/// @return                  Whether a decision was made
template <typename SocketBlackBox,typename SDAConfig>
bool TEMP_DECISIONMAKER::Decide(DriveSituation& p_driveSituation)
{
    DecisionTuple decision;
    if (!m_blackBox.GetDecisions(p_driveSituation, decision)) return false;

    m_interventionExecutor->RunDecision(decision.GetDecisions(), DECISIONS_COUNT);
    return true;
}

/// @brief         Changes the settings of how decisions should be made
/// @param  p_type The new type of interventions
template<typename SocketBlackBox,typename SDAConfig>
void TEMP_DECISIONMAKER::ChangeSettings(INTERVENTION_TYPE p_type)
{
    m_interventionExecutor = m_config.SetInterventionType(p_type);
}

/// @brief         Changes the settings of how decisions should be made
/// @param  p_type The new type of interventions
template<typename SocketBlackBox,typename SDAConfig>
void TEMP_DECISIONMAKER::SetDataCollectionSettings(bool* p_dataSetting)
{
    m_config.SetDataCollectionSettings(p_dataSetting);
}