#include "SteerDecision.h"
#include "Mediator.h"

void SteerDecision::RunIndicateCommands()
{
    if (SteerAmount < -SDA_STEERING_THRESHOLD)
    {
        SMediator::GetInstance()->CarController.ShowIntervention(INTERVENTION_ACTION_TURN_RIGHT);
        return;
    }
    if (SteerAmount > SDA_STEERING_THRESHOLD)
    {
        SMediator::GetInstance()->CarController.ShowIntervention(INTERVENTION_ACTION_TURN_LEFT);
    }
};

void SteerDecision::RunInterveneCommands(tAllowedActions p_allowedActions)
{
    if (SteerAmount > -SDA_STEERING_THRESHOLD && SteerAmount < SDA_STEERING_THRESHOLD) return;
    if (!p_allowedActions.Steer) return;
    SMediator::GetInstance()->CarController.SetSteerCmd(SteerAmount);
};