#pragma once
#include "InterventionExecutor.h"

/// @brief Always intervenes the simulation
class InterventionExecutorAlwaysIntervene : public InterventionExecutor
{
    void RunDecision(IDecision** p_decisions, int p_decisionCount) override;
};