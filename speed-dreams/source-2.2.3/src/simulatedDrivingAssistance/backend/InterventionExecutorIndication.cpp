#include "InterventionExecutorIndication.h"
#include "Mediator.h"

/// @brief                  Will only indicate an intervention to a participant
/// @param  p_decisions     The decisions
/// @param  p_decisionCount The amount of decisions
void InterventionExecutorIndication::RunDecision(IDecision** p_decisions, int p_decisionCount)
{
    SMediator::GetInstance()->CarController.ShowUI(INTERVENTION_ACTION_NONE);
    for (int i = 0; i < p_decisionCount; i++)
    {
        p_decisions[i]->RunIndicateCommands();
    }
}