#include "SteerDecision.h"
#include "Mediator.h"

#define STEERING_THRESHOLD 0.04f

void SteerDecision::RunIndicateCommands()
{
	if(m_steerAmount < -STEERING_THRESHOLD)
	{
		SMediator::GetInstance()->CarController.ShowUI(INTERVENTION_ACTION_TURN_RIGHT);
		return;
	}
	if(m_steerAmount > STEERING_THRESHOLD)
	{
		SMediator::GetInstance()->CarController.ShowUI(INTERVENTION_ACTION_TURN_LEFT);
	}
};
void SteerDecision::RunInterveneCommands()
{
	if (m_steerAmount > -STEERING_THRESHOLD && m_steerAmount < STEERING_THRESHOLD) return;

	SMediator::GetInstance()->CarController.SetSteerCmd(m_steerAmount);
};