#pragma once

#include <iostream>
#include "InterventionExecutor.h"

class InterventionExecutorAskFor : public InterventionExecutor
{
    void RunDecision(IDecision** p_decisions, int p_decisionCount) override;
};