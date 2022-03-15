#pragma once

#include <iostream>
#include "InterventionExecutor.h"

class InterventionExecutorNoIntervention : public InterventionExecutor
{
    void RunDecision(IDecision** p_decisions) override
    {
        std::cout << "Make no commands" << std::endl;
    }
};