#include "BrakeDecision.h"
#include "Mediator.h"
#define BRAKE_THRESHOLD 0.2f


void BrakeDecision::RunIndicateCommands()
{
	if (m_brakeAmount < BRAKE_THRESHOLD) return;

	SMediator::GetInstance()->CarController.ShowUI(INTERVENTION_ACTION_BRAKE);
}

void BrakeDecision::RunInterveneCommands()
{
	SMediator::GetInstance()->CarController.SetBrakeCmd(m_brakeAmount);
}