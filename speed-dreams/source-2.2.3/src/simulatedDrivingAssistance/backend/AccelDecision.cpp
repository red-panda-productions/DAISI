#include "AccelDecision.h"
#include "Mediator.h"

void AccelDecision::RunIndicateCommands()
{
    float threshold = SMediator::GetInstance()->GetThresholdSettings().Accel;
    if (AccelAmount < threshold && SMediator::GetInstance()->GetInterventionType() != INTERVENTION_TYPE_AUTONOMOUS_AI) return;
    if (!SMediator::GetInstance()->GetAllowedActions().Accelerate) return;

    SMediator::GetInstance()->CarControl.ShowIntervention(INTERVENTION_ACTION_SPEED_ACCEL);
}

/// @brief Runs the intervene commands
/// @param p_allowedActions The allowed black box actions, for determining whether or not the command may be ran
void AccelDecision::RunInterveneCommands(tAllowedActions p_allowedActions)
{
    float threshold = SMediator::GetInstance()->GetThresholdSettings().Accel;
    if (AccelAmount < threshold || !p_allowedActions.Accelerate)
    {
        SMediator::GetInstance()->SetAccelDecision(false);
        return;
    }

    SMediator::GetInstance()->CarControl.SetAccelCmd(AccelAmount);
}