#include "AccelDecision.h"
#include "Mediator.h"

/// @brief Shows the intervention on the screen
void AccelDecision::ShowIntervention(float p_interventionAmount)
{
    SMediator::GetInstance()->CarControl.ShowIntervention(INTERVENTION_ACTION_SPEED_ACCEL);
}

/// @brief Runs the intervene commands
/// @param p_interventionAmount The intervention amount
void AccelDecision::DoIntervention(float p_interventionAmount)
{
    SMediator::GetInstance()->CarControl.SetAccelCmd(p_interventionAmount);
}

/// @brief tells whether the intervention amount is higher than the threshold
/// @param p_interventionAmount The intervention amount
/// @return whether the threshold is reached
bool AccelDecision::ReachThreshold(float p_interventionAmount)
{
    float threshold = SMediator::GetInstance()->GetThresholdSettings().Accel;
    return threshold < p_interventionAmount;
}

/// @brief tells whether the simulator can be intervened by the decision
/// @param p_allowedActions The allowed black box actions
/// @return whether the simulator can be intervened
bool AccelDecision::CanIntervene(tAllowedActions p_allowedActions)
{
    return p_allowedActions.Accelerate;
}
