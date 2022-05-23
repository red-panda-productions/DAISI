#include "InterventionExecutorIndication.h"
#include "Mediator.h"

/// @brief                  Will only indicate an intervention to a participant
/// @param  p_decisions     The decisions
/// @param  p_decisionCount The amount of decisions
void InterventionExecutorIndication::RunDecision(IDecision** p_decisions, int p_decisionCount)
{
    SMediator* mediator = SMediator::GetInstance();
    mediator->CarController.ShowIntervention(INTERVENTION_ACTION_STEER_NEUTRAL);
    mediator->CarController.ShowIntervention(INTERVENTION_ACTION_SPEED_NEUTRAL);

    for (int i = 0; i < p_decisionCount; i++)
    {
        p_decisions[i]->RunIndicateCommands();
    }
}