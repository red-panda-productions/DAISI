#include "SteerDecision.h"
#include "Mediator.h"

void SteerDecision::RunIndicateCommands()
{
	if(m_steerAmount < -0.3f)
	{
		SMediator::GetInstance()->CarController.ShowUI(INTERVENTION_ACTION_TURN_LEFT);
		return;
	}
	if(m_steerAmount > 0.3f)
	{
		SMediator::GetInstance()->CarController.ShowUI(INTERVENTION_ACTION_TURN_RIGHT);
	}
};
void SteerDecision::RunInterveneCommands()
{

};