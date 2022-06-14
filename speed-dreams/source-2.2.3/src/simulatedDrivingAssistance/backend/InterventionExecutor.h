#pragma once
#include "Decision.h"

/// @brief A class that can execute actions based on decisions
class InterventionExecutor
{
public:
    /// @brief                  Runs an array of decisions made by the black box
    /// @param  p_decisions     The decisions that were made
    /// @param  p_decisionCount The amount of decisions
    virtual void RunDecision(Decision** p_decisions, int p_decisionCount) = 0;

    virtual ~InterventionExecutor() = default;
};