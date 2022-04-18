#pragma once
#include "InterventionExecutor.h"

class InterventionExecutorMock : public InterventionExecutor
{
public:
    void RunDecision(IDecision** p_decisions, int p_decisionCount) override
    {
        Decisions = p_decisions;
        DecisionCount = p_decisionCount;
    }

    IDecision** Decisions = nullptr;
    int DecisionCount = 0;
};