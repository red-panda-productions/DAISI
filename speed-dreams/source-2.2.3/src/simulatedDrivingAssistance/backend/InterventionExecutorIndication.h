/*
 * This program has been developed by students from the bachelor Computer Science at
 * Utrecht University within the Software Project course.
 * © Copyright Utrecht University (Department of Information and Computing Sciences)
 */

#pragma once
#include "InterventionExecutor.h"

/// @brief Shows indicators for interventions
class InterventionExecutorIndication : public InterventionExecutor
{
    void RunDecision(Decision** p_decisions, int p_decisionCount) override;
};