#include "InterventionExecutorPerformWhenNeeded.h"
#include "Mediator.h"

/// @brief                  Will only intervene the simulation when needed
/// @param  p_decisions     The decisions
/// @param  p_decisionCount The amount of decisions
void InterventionExecutorPerformWhenNeeded::RunDecision(Decision** p_decisions, int p_decisionCount)
{
    SMediator* mediator = SMediator::GetInstance();
    mediator->CarControl.ShowIntervention(INTERVENTION_ACTION_STEER_NEUTRAL);
    mediator->CarControl.ShowIntervention(INTERVENTION_ACTION_SPEED_NEUTRAL);

    tAllowedActions allowedActions = mediator->GetAllowedActions();
    for (int i = 0; i < p_decisionCount; i++)
    {
        p_decisions[i]->RunInterveneCommands(allowedActions);
        p_decisions[i]->RunIndicateCommands();
    }
}