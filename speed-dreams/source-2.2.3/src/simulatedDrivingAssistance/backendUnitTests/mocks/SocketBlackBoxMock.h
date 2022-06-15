#pragma once
#include "DecisionTuple.h"
#include "BlackBoxData.h"

class SocketBlackBoxMock
{
public:
    SocketBlackBoxMock()
    {
        Decisions.SetSteerDecision(20);
        Decisions.SetBrakeDecision(40);
        Decisions.SetAccelDecision(60);
    }

    void Initialize(bool p_connectAsync, BlackBoxData& p_initialDriveSituation, BlackBoxData* p_tests = nullptr, int p_amountOfTests = 0)
    {
        InitialDriveSituation = new BlackBoxData(p_initialDriveSituation);
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
        return InitialDriveSituation;
    }

    ~SocketBlackBoxMock()
    {
        delete InitialDriveSituation;
    }

    bool IsDecision = false;
    BlackBoxData* InitialDriveSituation = nullptr;
    DecisionTuple Decisions;
};