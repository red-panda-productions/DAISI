#include "AccelDecision.h"
#include "Mediator.h"

void AccelDecision::RunIndicateCommands()
{
    if (AccelAmount < SMediator::GetInstance()->GetThresholdSettings().Accel) return;

    SMediator::GetInstance()->CarController.ShowIntervention(INTERVENTION_ACTION_SPEED_ACCEL);
}

/// @brief Runs the intervene commands
/// @param p_allowedActions The allowed black box actions, for determining whether or not the command may be ran
void AccelDecision::RunInterveneCommands(tAllowedActions p_allowedActions)
{
    if (!p_allowedActions.Accelerate) return;
    SMediator::GetInstance()->CarController.SetAccelCmd(AccelAmount);
}