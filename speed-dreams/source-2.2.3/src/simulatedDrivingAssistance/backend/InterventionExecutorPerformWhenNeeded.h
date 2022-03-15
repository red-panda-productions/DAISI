#pragma once

#include <iostream>
#include "InterventionExecutor.h"

/// @brief Only intervenes if it is necessary to intervene
class InterventionExecutorPerformWhenNeeded : public InterventionExecutor
{
    void RunDecision(IDecision** p_decisions, int p_decisionCount) override;
};