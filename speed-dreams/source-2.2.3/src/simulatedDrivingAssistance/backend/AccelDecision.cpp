#include "AccelDecision.h"
#include "Mediator.h"

void AccelDecision::RunIndicateCommands()
{
    // TODO: Implement acceleration indication
    if (AccelAmount < ACCEL_THRESHOLD) return;

    SMediator::GetInstance()->CarController.ShowIntervention(INTERVENTION_ACTION_ACCELERATE);
}

void AccelDecision::RunInterveneCommands()
{
    SMediator::GetInstance()->CarController.SetAccelCmd(AccelAmount);
}