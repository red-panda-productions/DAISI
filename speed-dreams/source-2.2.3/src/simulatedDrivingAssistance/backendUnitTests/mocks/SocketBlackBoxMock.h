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
        Decisions.SetAccel(60);
    }

    void Initialize(BlackBoxData& p_initialDriveSituation, BlackBoxData* p_tests = nullptr, int p_amountOfTests = 0)
    {
        initialDriveSituation = &p_initialDriveSituation;
    }

    void Initialize()
    {

    }

    
    void Shutdown()
    {
    }

    bool GetDecisions(tCarElt* p_car, tSituation* p_situation, unsigned long p_tickCount, DecisionTuple& p_decisions) const
    {
        if (!IsDecision) return false;
        p_decisions = Decisions;
        return true;
    }

    BlackBoxData* GetBlackBoxData() const
    {

        return initialDriveSituation;
    }

    bool IsDecision = false;
    BlackBoxData* initialDriveSituation = nullptr;
    DecisionTuple Decisions;
};