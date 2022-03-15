#pragma once

#include <iostream>
#include "InterventionExecutor.h"

class InterventionExecutorAlwaysIntervene : public InterventionExecutor
{
    void RunDecision(IDecision** p_decisions) override
    {
        std::cout << "Make commands that always intervene" << std::endl;
    }
};