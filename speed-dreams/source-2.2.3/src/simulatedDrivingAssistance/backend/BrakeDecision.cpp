#include "BrakeDecision.h"
#include "Mediator.h"

void BrakeDecision::RunIndicateCommands()
{
    if (BrakeAmount < BRAKE_THRESHOLD) return;

    SMediator::GetInstance()->CarController.ShowIntervention(INTERVENTION_ACTION_SPEED_BRAKE);
}

/// @brief Runs the intervene commands
/// @param p_allowedActions The allowed black box actions, for determining whether or not the command may be rans
void BrakeDecision::RunInterveneCommands(tAllowedActions p_allowedActions)
{
    if (!p_allowedActions.Brake) return;
    SMediator::GetInstance()->CarController.SetBrakeCmd(BrakeAmount);
}