#pragma once
#include "IDecision.h"
class InterventionExecutor
{
public:
    /// @brief                  Runs an array of decisions made by the black box
    /// @param  p_decisions     The decisions that were made
    /// @param  p_decisionCount The amount of decisions
    virtual void RunDecision(IDecision** p_decisions, int p_decisionCount) = 0;
};