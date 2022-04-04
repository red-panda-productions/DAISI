#pragma once
#include "DecisionTuple.h"
#include "BlackBoxData.h"

class SocketBlackBoxMock
{
public:
	SocketBlackBoxMock()
	{
		Decisions.SetSteer(20);
		Decisions.SetBrake(40);
	}

	void Initialize(BlackBoxData& p_initialDriveSituation, BlackBoxData* p_tests = nullptr, int p_amountOfTests = 0)
	{

	}

	void Initialize()
	{

	}

	bool GetDecisions(tCarElt* p_car, tSituation* p_situation, int p_tickCount, DecisionTuple& p_decisions)
	{
		if (!IsDecision) return false;
		p_decisions = Decisions;
		return true;
	}

	bool IsDecision = false;

	DecisionTuple Decisions;

};