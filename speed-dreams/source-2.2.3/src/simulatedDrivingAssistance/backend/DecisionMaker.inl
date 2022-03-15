#pragma once
#include "DecisionMaker.h"
#include "DecisionTuple.h"

#define CREATE_DECISION_MAKER_IMPLEMENTATION(type1,type2) \
    template bool DecisionMaker<type1,type2>::Decide(DriveSituation& p_driveSituation);\
    template void DecisionMaker<type1,type2>::ChangeSettings(INTERVENTION_TYPE p_type);

#define TEMP_DECISIONMAKER DecisionMaker<SocketBlackBox,Config>

/// @brief                   Tries to get a decision from the black box
/// @param  p_driveSituation The current drive situation
/// @return                  Whether a decision was made
template <typename SocketBlackBox,typename Config>
bool TEMP_DECISIONMAKER::Decide(DriveSituation& p_driveSituation)
{
    DecisionTuple decision;
    if (!m_blackBox.GetDecisions(p_driveSituation, decision)) return false;

    m_interventionExecutor->RunDecision(decision.GetDecisions(), DECISIONS_COUNT);
    return true;
}

/// @brief         Changes the settings of how decisions should be made
/// @param  p_type The new type of interventions
template<typename SocketBlackBox,typename Config>
void TEMP_DECISIONMAKER::ChangeSettings(INTERVENTION_TYPE p_type)
{
    m_interventionExecutor = m_config.SetInterventionType(p_type);
}