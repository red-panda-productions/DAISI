/*
 * This program has been developed by students from the bachelor Computer Science at
 * Utrecht University within the Software Project course.
 * © Copyright Utrecht University (Department of Information and Computing Sciences)
 */

#include "InterventionExecutorAlwaysIntervene.h"
#include "Mediator.h"

/// @brief                  Will always intervene if the black box decided anything
/// @param  p_decisions     The decisions
/// @param  p_decisionCount The amount of decisions
void InterventionExecutorAlwaysIntervene::RunDecision(Decision** p_decisions, int p_decisionCount)
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