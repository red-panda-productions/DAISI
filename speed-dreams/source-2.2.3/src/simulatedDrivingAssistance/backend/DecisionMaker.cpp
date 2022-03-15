#include "DecisionMaker.h"
#include "DecisionTuple.h"


#define CREATE_IMPLEMENTATION(type) \
    template bool DecisionMaker<type>::Decide(DriveSituation& p_driveSituation);\
    template void DecisionMaker<type>::ChangeSettings(INTERVENTION_TYPE p_type);


template <typename BlackBox>
bool DecisionMaker<BlackBox>::Decide(DriveSituation& p_driveSituation)
{
    DecisionTuple decision;
    if (!m_blackBox.GetDecisions(p_driveSituation, decision)) return false;
    // perform commands?

    decision.m_steerDecision.GetIndicateCommands();
    decision.m_steerDecision.GetInterveneCommands();



}

template<typename BlackBox>
void DecisionMaker<BlackBox>::ChangeSettings(INTERVENTION_TYPE p_type)
{
    m_interventionMaker = m_config.SetInterventionType(p_type);

    // this line is for debugging purposes only, should be removed
    m_interventionMaker->MakeCommands();
}

CREATE_IMPLEMENTATION(SSocketBlackBox)