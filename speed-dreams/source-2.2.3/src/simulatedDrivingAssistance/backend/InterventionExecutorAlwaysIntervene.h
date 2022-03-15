#pragma once

#include <iostream>
#include "InterventionExecutor.h"

class InterventionExecutorAlwaysIntervene : public InterventionExecutor
{
    void RunDecision(IDecision** p_decisions, int p_decisionCount) override;
};