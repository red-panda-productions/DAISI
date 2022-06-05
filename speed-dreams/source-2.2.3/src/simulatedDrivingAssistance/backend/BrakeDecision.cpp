#include "BrakeDecision.h"
#include "Mediator.h"

void BrakeDecision::RunIndicateCommands()
{
    float threshold = SMediator::GetInstance()->GetThresholdSettings().Brake;
    if (BrakeAmount < threshold && SMediator::GetInstance()->GetInterventionType() != INTERVENTION_TYPE_AUTONOMOUS_AI) return;
    if (!SMediator::GetInstance()->GetAllowedActions().Brake) return;

    SMediator::GetInstance()->CarControl.ShowIntervention(INTERVENTION_ACTION_SPEED_BRAKE);
}

/// @brief Runs the intervene commands
/// @param p_allowedActions The allowed black box actions, for determining whether or not the command may be rans
void BrakeDecision::RunInterveneCommands(tAllowedActions p_allowedActions)
{
    float threshold = SMediator::GetInstance()->GetThresholdSettings().Brake;
    if (BrakeAmount < threshold || !p_allowedActions.Brake)
    {
        SMediator::GetInstance()->SetBrakeDecision(false);
        return;
    }

    SMediator::GetInstance()->CarControl.SetBrakeCmd(BrakeAmount);
}