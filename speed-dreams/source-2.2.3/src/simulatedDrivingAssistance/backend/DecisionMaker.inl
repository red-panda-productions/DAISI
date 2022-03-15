#pragma once
#include "DecisionMaker.h"
#include "DecisionTuple.h"

#define CREATE_DECISION_MAKER_IMPLEMENTATION(type1,type2) \
    template bool DecisionMaker<type1,type2>::Decide(DriveSituation& p_driveSituation);\
    template void DecisionMaker<type1,type2>::ChangeSettings(INTERVENTION_TYPE p_type);

#define TEMP_DECISIONMAKER DecisionMaker<SocketBlackBox,Config>

template <typename SocketBlackBox,typename Config>
bool TEMP_DECISIONMAKER::Decide(DriveSituation& p_driveSituation)
{
    DecisionTuple decision;
    if (!m_blackBox.GetDecisions(p_driveSituation, decision)) return false;

    m_interventionExecutor->RunDecision(decision.GetDecisions(), DECISIONS_COUNT);
    return true;
}

template<typename SocketBlackBox,typename Config>
void TEMP_DECISIONMAKER::ChangeSettings(INTERVENTION_TYPE p_type)
{
    m_interventionExecutor = m_config.SetInterventionType(p_type);
}