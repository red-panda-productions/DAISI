/*
 * This program has been developed by students from the bachelor Computer Science at
 * Utrecht University within the Software Project course.
 * © Copyright Utrecht University (Department of Information and Computing Sciences)
 */

#include "BrakeDecision.h"
#include "Mediator.h"

/// @brief Shows the intervention on the screen
void BrakeDecision::ShowIntervention(float p_interventionAmount)
{
    SMediator::GetInstance()->CarControl.ShowIntervention(INTERVENTION_ACTION_SPEED_BRAKE);
}

/// @brief Runs the intervene commands
/// @param p_interventionAmount The intervention amount
void BrakeDecision::DoIntervention(float p_interventionAmount)
{
    SMediator::GetInstance()->CarControl.SetBrakeCmd(p_interventionAmount);
}

/// @brief tells whether the intervention amount is higher than the threshold
/// @param p_interventionAmount The intervention amount
/// @return whether the threshold is reached
bool BrakeDecision::ReachThreshold(float p_interventionAmount)
{
    float threshold = SMediator::GetInstance()->GetThresholdSettings().Brake;
    return threshold < p_interventionAmount;
}

/// @brief tells whether the simulator can be intervened by the decision
/// @param p_allowedActions The allowed black box actions
/// @return whether the simulator can be intervened
bool BrakeDecision::CanIntervene(tAllowedActions p_allowedActions)
{
    return p_allowedActions.Brake;
}