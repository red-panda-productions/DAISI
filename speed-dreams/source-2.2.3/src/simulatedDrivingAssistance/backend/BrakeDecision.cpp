#include "BrakeDecision.h"
#include "Mediator.h"

void BrakeDecision::RunIndicateCommands()
{
    if (BrakeAmount < BRAKE_THRESHOLD) return;

    SMediator::GetInstance()->CarController.ShowIntervention(INTERVENTION_ACTION_BRAKE);
}

void BrakeDecision::RunInterveneCommands(tAllowedActions p_allowedActions)
{
    if (!p_allowedActions.Brake) return;
    SMediator::GetInstance()->CarController.SetBrakeCmd(BrakeAmount);
}