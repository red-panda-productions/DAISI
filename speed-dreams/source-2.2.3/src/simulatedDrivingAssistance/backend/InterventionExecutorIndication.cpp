#include "InterventionExecutorIndication.h"

void InterventionExecutorIndication::RunDecision(IDecision** p_decisions, int p_decisionCount)
{
    for (int i = 0; i < p_decisionCount; i++)
    {
        p_decisions[i]->RunIndicateCommands();
    }
}