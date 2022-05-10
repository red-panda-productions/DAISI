#include "AccelDecision.h"
#include "Mediator.h"

void AccelDecision::RunIndicateCommands()
{
    if (AccelAmount < SMediator::GetInstance()->GetThresholdSettings().Accel) return;

    SMediator::GetInstance()->CarController.ShowIntervention(INTERVENTION_ACTION_ACCELERATE);
}

void AccelDecision::RunInterveneCommands()
{
    SMediator::GetInstance()->CarController.SetAccelCmd(AccelAmount);
}