#pragma once
#include "DriveSituationMock.h"
#include "DecisionTuple.h"

class SocketBlackBoxMock
{
public:
	SocketBlackBoxMock()
	{
		Decisions.SetSteer(20);
		Decisions.SetBrake(40);
	}

	void Initialize(DriveSituation& p_initialDriveSituation, DriveSituation* p_tests = nullptr, int p_amountOfTests = 0)
	{
		
	}

	void Initialize()
	{
		
	}

	bool GetDecisions(DriveSituation& driveSituation, DecisionTuple& p_decisions)
	{
		if (!IsDecision) return false;
		p_decisions = Decisions;
		return true;
	}

	bool IsDecision = false;

	DecisionTuple Decisions;

};