#include "BrakeDecision.h"
#include "Mediator.h"

void BrakeDecision::RunIndicateCommands()
{
    if (BrakeAmount < SMediator::GetInstance()->GetThresholdSettings().Brake && SMediator::GetInstance()->GetInterventionType() != INTERVENTION_TYPE_AUTONOMOUS_AI) return;

    SMediator::GetInstance()->CarController.ShowIntervention(INTERVENTION_ACTION_SPEED_BRAKE);
}

/// @brief Runs the intervene commands
/// @param p_allowedActions The allowed black box actions, for determining whether or not the command may be rans
void BrakeDecision::RunInterveneCommands(tAllowedActions p_allowedActions)
{
    if (BrakeAmount < SMediator::GetInstance()->GetThresholdSettings().Brake || !p_allowedActions.Brake)
    {
        SMediator::GetInstance()->SetBrakeDecision(false);
        return;
    }

    SMediator::GetInstance()->CarController.SetBrakeCmd(BrakeAmount);
}