#include "BrakeDecision.h"
#include "Mediator.h"
#define BRAKE_THRESHOLD 0.9f

template <class Mediator>
void BrakeDecision<Mediator>::RunIndicateCommands()
{
    if (BrakeAmount < BRAKE_THRESHOLD) return;

	Mediator::GetInstance()->CarController.ShowIntervention(INTERVENTION_ACTION_BRAKE);
}

template <class Mediator>
void BrakeDecision<Mediator>::RunInterveneCommands()
{
	Mediator::GetInstance()->CarController.SetBrakeCmd(m_brakeAmount);
}