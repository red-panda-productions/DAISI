#pragma once

#include <iostream>
#include "InterventionExecutor.h"

/// @brief Asks the participant if it may intervene
class InterventionExecutorAskFor : public InterventionExecutor
{
    void RunDecision(IDecision** p_decisions, int p_decisionCount) override;
};