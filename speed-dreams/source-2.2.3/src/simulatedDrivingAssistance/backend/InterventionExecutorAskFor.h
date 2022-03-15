#pragma once

#include <iostream>
#include "InterventionExecutor.h"

class InterventionExecutorAskFor : public InterventionExecutor
{
    void RunDecision(IDecision** p_decisions) override
    {
        std::cout << "Make commands that ask to be done" << std::endl;
    }
};