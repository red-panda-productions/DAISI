#pragma once

#include <iostream>
#include "InterventionExecutor.h"

class InterventionExecutorIndication : public InterventionExecutor
{
    void RunDecision(IDecision** p_decisions) override
    {
        std::cout << "Make commands that show indication" << std::endl;
    }
};