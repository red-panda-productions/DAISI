/*
 * This program has been developed by students from the bachelor Computer Science at
 * Utrecht University within the Software Project course.
 * © Copyright Utrecht University (Department of Information and Computing Sciences)
 */

#pragma once
#include "InterventionExecutor.h"

/// @brief Only intervenes if it is necessary to intervene
class InterventionExecutorPerformWhenNeeded : public InterventionExecutor
{
    void RunDecision(Decision** p_decisions, int p_decisionCount) override;
};