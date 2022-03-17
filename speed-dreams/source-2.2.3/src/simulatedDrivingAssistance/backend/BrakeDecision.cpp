#include "BrakeDecision.h"
#include "Mediator.h"
void BrakeDecision::RunIndicateCommands()
{
	if (m_brakeAmount < 0.5f) return;

	SMediator::GetInstance().CarController.ShowUI(UI_TYPE_BRAKE);
}

void BrakeDecision::RunInterveneCommands()
{

}