#include "AccelDecision.h"
#include "Mediator.h"

void AccelDecision::RunIndicateCommands()
{
    if (AccelAmount < ACCEL_THRESHOLD) return;

    SMediator::GetInstance()->CarController.ShowIntervention(INTERVENTION_ACTION_ACCELERATE);
}

void AccelDecision::RunInterveneCommands(tAllowedActions p_allowedActions)
{
    if (!p_allowedActions.Accelerate) return;
    SMediator::GetInstance()->CarController.SetAccelCmd(AccelAmount);
}