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

    m_interventionExecutor->RunDecision(decision.GetDecisions());
}

template<typename BlackBox>
void DecisionMaker<BlackBox>::ChangeSettings(INTERVENTION_TYPE p_type)
{
    m_interventionExecutor = m_config.SetInterventionType(p_type);
}

CREATE_IMPLEMENTATION(SSocketBlackBox)