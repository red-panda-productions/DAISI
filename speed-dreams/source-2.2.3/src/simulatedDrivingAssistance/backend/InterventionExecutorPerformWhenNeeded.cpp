#include "InterventionExecutorPerformWhenNeeded.h"

void InterventionExecutorPerformWhenNeeded::RunDecision(IDecision** p_decisions, int p_decisionCount)
{
    for (int i = 0; i < p_decisionCount; i++)
    {
        p_decisions[i]->RunInterveneCommands();
        p_decisions[i]->RunIndicateCommands();
    }
}