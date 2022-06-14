#pragma once
#include "InterventionExecutor.h"

class InterventionExecutorMock : public InterventionExecutor
{
public:
    void RunDecision(Decision** p_decisions, int p_decisionCount) override
    {
        Decisions = p_decisions;
        DecisionCount = p_decisionCount;
    }

    Decision** Decisions = nullptr;
    int DecisionCount = 0;
};