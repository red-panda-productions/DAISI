#pragma once

#include <iostream>
#include "InterventionExecutor.h"

class InterventionExecutorPerformWhenNeeded : public InterventionExecutor
{
    void RunDecision(IDecision** p_decisions, int p_decisionCount) override;
};