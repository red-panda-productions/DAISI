#include "InterventionExecutorPerformWhenNeeded.h"
#include "Mediator.h"

void InterventionExecutorPerformWhenNeeded::RunDecision(IDecision** p_decisions, int p_decisionCount)
{
    SMediator::GetInstance()->CarController.ShowUI(INTERVENTION_ACTION_NONE);
    for (int i = 0; i < p_decisionCount; i++)
    {
        p_decisions[i]->RunInterveneCommands();
        p_decisions[i]->RunIndicateCommands();
    }
}