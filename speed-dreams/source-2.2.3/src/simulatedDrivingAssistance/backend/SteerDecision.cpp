#include "SteerDecision.h"
#include "Mediator.h"

void SteerDecision::RunIndicateCommands()
{
    float threshold = SMediator::GetInstance()->GetThresholdSettings().Steer;
    if (SteerAmount < -threshold)
    {
        SMediator::GetInstance()->CarController.ShowIntervention(INTERVENTION_ACTION_TURN_RIGHT);
        return;
    }
    if (SteerAmount > threshold)
    {
        SMediator::GetInstance()->CarController.ShowIntervention(INTERVENTION_ACTION_TURN_LEFT);
    }
};

void SteerDecision::RunInterveneCommands()
{
    float threshold = SMediator::GetInstance()->GetThresholdSettings().Steer;
    if (SteerAmount > -threshold && SteerAmount < threshold) return;

    SMediator::GetInstance()->CarController.SetSteerCmd(SteerAmount);
};