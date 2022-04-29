#include "BrakeDecision.h"
#include "Mediator.h"

void BrakeDecision::RunIndicateCommands()
{
    if (BrakeAmount < BRAKE_THRESHOLD) return;

    SMediator::GetInstance()->CarController.ShowIntervention(INTERVENTION_ACTION_BRAKE);
}

void BrakeDecision::RunInterveneCommands()
{
    SMediator::GetInstance()->CarController.SetBrakeCmd(BrakeAmount);
}