#pragma once
#include "IDecision.h"
class InterventionExecutor
{
public:
    // returns an array of commands based on the decisions made by the black box and the intervention type
    virtual void RunDecision(IDecision** p_decisions) = 0;
};