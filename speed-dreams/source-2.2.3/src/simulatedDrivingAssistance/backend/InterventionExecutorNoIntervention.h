#pragma once
#include "InterventionExecutor.h"

/// @brief Never intervenes the simulation
class InterventionExecutorNoIntervention : public InterventionExecutor
{
    void RunDecision(IDecision** p_decisions, int p_decisionCount) override;
};