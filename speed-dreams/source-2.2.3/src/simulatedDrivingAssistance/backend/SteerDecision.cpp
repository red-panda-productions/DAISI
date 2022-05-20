#include "SteerDecision.h"
#include "Mediator.h"

void SteerDecision::RunIndicateCommands()
{
    if (SteerAmount < -SDA_STEERING_THRESHOLD)
    {
        SMediator::GetInstance()->CarController.ShowIntervention(INTERVENTION_ACTION_STEER_RIGHT);
        return;
    }
    if (SteerAmount > SDA_STEERING_THRESHOLD)
    {
        SMediator::GetInstance()->CarController.ShowIntervention(INTERVENTION_ACTION_STEER_LEFT);
    }
};

/// @brief Runs the intervene commands
/// @param p_allowedActions The allowed black box actions, for determining whether or not the command may be ran
void SteerDecision::RunInterveneCommands(tAllowedActions p_allowedActions)
{
    if (SteerAmount > -SDA_STEERING_THRESHOLD && SteerAmount < SDA_STEERING_THRESHOLD) return;
    if (!p_allowedActions.Steer) return;
    SMediator::GetInstance()->CarController.SetSteerCmd(SteerAmount);
};