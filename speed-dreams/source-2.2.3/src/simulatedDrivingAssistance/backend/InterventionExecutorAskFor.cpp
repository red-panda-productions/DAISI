#include "InterventionExecutorAskFor.h"
#include <iostream>  // remove this on implementation

/// @brief					Will ask the participant if it may intervene
/// @param  p_decisions     The decisions
/// @param  p_decisionCount The amount of decisions
void InterventionExecutorAskFor::RunDecision(IDecision** p_decisions, int p_decisionCount)
{
    std::cout << "Make commands that ask to be done" << std::endl;
}
