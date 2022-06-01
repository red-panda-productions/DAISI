#include "SteerDecision.h"
#include "Mediator.h"

void SteerDecision::RunIndicateCommands()
{
    float threshold = SMediator::GetInstance()->GetThresholdSettings().Steer;
    if (SteerAmount <= -threshold)
    {
        SMediator::GetInstance()->CarControl.ShowIntervention(INTERVENTION_ACTION_STEER_RIGHT);
        return;
    }
    if (SteerAmount >= threshold)
    {
        SMediator::GetInstance()->CarControl.ShowIntervention(INTERVENTION_ACTION_STEER_LEFT);
    }
};

/// @brief Runs the intervene commands
/// @param p_allowedActions The allowed black box actions, for determining whether or not the command may be ran
void SteerDecision::RunInterveneCommands(tAllowedActions p_allowedActions)
{
    float threshold = SMediator::GetInstance()->GetThresholdSettings().Steer;
    if (SteerAmount > -threshold && SteerAmount < threshold) return;
    if (!p_allowedActions.Steer) return;
    SMediator::GetInstance()->CarControl.SetSteerCmd(SteerAmount);
};