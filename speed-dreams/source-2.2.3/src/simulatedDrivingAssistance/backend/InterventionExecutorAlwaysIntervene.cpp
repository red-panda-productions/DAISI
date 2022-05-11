#include "InterventionExecutorAlwaysIntervene.h"
#include "Mediator.h"

/// @brief                  Will always intervene if the black box decided anything
/// @param  p_decisions     The decisions
/// @param  p_decisionCount The amount of decisions
void InterventionExecutorAlwaysIntervene::RunDecision(IDecision** p_decisions, int p_decisionCount)
{
    SMediator::GetInstance()->CarController.ShowIntervention(INTERVENTION_ACTION_STEER_NONE);
    for (int i = 0; i < p_decisionCount; i++)
    {
        p_decisions[i]->RunInterveneCommands();
        p_decisions[i]->RunIndicateCommands();
    }
}