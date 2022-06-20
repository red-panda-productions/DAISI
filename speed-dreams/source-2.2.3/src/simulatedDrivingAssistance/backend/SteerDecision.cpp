/*
 * This program has been developed by students from the bachelor Computer Science at
 * Utrecht University within the Software Project course.
 * © Copyright Utrecht University (Department of Information and Computing Sciences)
 */

#include "SteerDecision.h"
#include "Mediator.h"

/// @brief Shows the intervention on the screen
void SteerDecision::ShowIntervention(float p_interventionAmount)
{
    float threshold = SMediator::GetInstance()->GetThresholdSettings().Steer;

    // if negative the AI is steering right
    if (p_interventionAmount < -threshold)
    {
        SMediator::GetInstance()->CarControl.ShowIntervention(INTERVENTION_ACTION_STEER_RIGHT);
        return;
    }

    // if positive the AI is steering left
    if (p_interventionAmount > threshold)
    {
        SMediator::GetInstance()->CarControl.ShowIntervention(INTERVENTION_ACTION_STEER_LEFT);
        return;
    }

    // if within the threshold bounds, the AI is steering straight
    SMediator::GetInstance()->CarControl.ShowIntervention(INTERVENTION_ACTION_STEER_STRAIGHT);
}

/// @brief Runs the intervene commands
/// @param p_interventionAmount The intervention amount
void SteerDecision::DoIntervention(float p_interventionAmount)
{
    SMediator::GetInstance()->CarControl.SetSteerCmd(p_interventionAmount);
}

/// @brief tells whether the intervention amount is higher than the threshold
/// @param p_interventionAmount The intervention amount
/// @return whether the threshold is reached
bool SteerDecision::ReachThreshold(float p_interventionAmount)
{
    float threshold = SMediator::GetInstance()->GetThresholdSettings().Steer;
    return threshold < abs(p_interventionAmount);
}

/// @brief tells whether the simulator can be intervened by the decision
/// @param p_allowedActions The allowed black box actions
/// @return whether the simulator can be intervened
bool SteerDecision::CanIntervene(tAllowedActions p_allowedActions)
{
    return p_allowedActions.Steer;
}