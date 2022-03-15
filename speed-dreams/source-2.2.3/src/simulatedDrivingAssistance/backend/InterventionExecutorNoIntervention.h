#pragma once

#include <iostream>
#include "InterventionExecutor.h"

class InterventionExecutorNoIntervention : public InterventionExecutor
{
    void RunDecision(IDecision** p_decisions, int p_decisionCount) override
    {
        // no commands
    }
};