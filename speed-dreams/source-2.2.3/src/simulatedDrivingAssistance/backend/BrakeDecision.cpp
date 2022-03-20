#include "BrakeDecision.h"
#include "Mediator.h"

void BrakeDecision::RunIndicateCommands()
{
	if (m_brakeAmount < 0.5f) return;

	SMediator::GetInstance()->CarController.ShowUI(INTERVENTION_ACTION_BRAKE);
}

void BrakeDecision::RunInterveneCommands()
{
	SMediator::GetInstance()->CarController.SetBrakeCmd(m_brakeAmount);
}