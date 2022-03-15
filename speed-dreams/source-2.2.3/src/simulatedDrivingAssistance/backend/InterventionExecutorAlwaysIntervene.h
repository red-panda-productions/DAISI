#pragma once

#include <iostream>
#include "InterventionExecutor.h"

class InterventionExecutorAlwaysIntervene : public InterventionExecutor
{
    void RunDecision(IDecision** p_decisions, int p_decisionCount) override
    {
        for(int i = 0; i < p_decisionCount; i++)
        {
            p_decisions[i]->RunInterveneCommands();
            p_decisions[i]->RunIndicateCommands();
        }
    }
};