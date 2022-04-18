#include "SteerDecision.h"
#include "Mediator.h"

#define SDA_STEERING_THRESHOLD 0.04f

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

void SteerDecision::RunInterveneCommands()
{
    if (SteerAmount > -SDA_STEERING_THRESHOLD && SteerAmount < SDA_STEERING_THRESHOLD) return;

    SMediator::GetInstance()->CarController.SetSteerCmd(SteerAmount);
};