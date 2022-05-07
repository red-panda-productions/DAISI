#include "BrakeDecision.h"
#include "Mediator.h"

void BrakeDecision::RunIndicateCommands()
{
    if (BrakeAmount < SMediator::GetInstance()->GetThresholdSettings().Brake) return;

    SMediator::GetInstance()->CarController.ShowIntervention(INTERVENTION_ACTION_BRAKE);
}

void BrakeDecision::RunInterveneCommands()
{
    SMediator::GetInstance()->CarController.SetBrakeCmd(BrakeAmount);
}