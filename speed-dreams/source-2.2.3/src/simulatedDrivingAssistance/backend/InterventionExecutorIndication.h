#pragma once
#include "InterventionExecutor.h"

/// @brief Shows indicators for interventions
class InterventionExecutorIndication : public InterventionExecutor
{
    void RunDecision(Decision** p_decisions, int p_decisionCount) override;
};