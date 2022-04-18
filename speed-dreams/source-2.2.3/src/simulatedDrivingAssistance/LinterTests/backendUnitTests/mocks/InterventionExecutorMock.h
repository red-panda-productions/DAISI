#pragma once
#include "InterventionExecutor.h"

class InterventionExecutorMock : public InterventionExecutor
{
public:
    virtual void RunDecision(IDecision** p_decisions, int p_decisionCount) override
    {
        m_decisions = p_decisions;
        m_decisionCount = p_decisionCount;
    }

    IDecision** m_decisions;
    int m_decisionCount;
};