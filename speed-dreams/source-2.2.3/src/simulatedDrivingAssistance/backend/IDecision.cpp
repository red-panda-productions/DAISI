#include "IDecision.h"
#include "Mediator.h"

/// @brief Can show an indicator
void IDecision::RunIndicateCommands()
{
    bool autonomousDriving = SMediator::GetInstance()->GetInterventionType() == INTERVENTION_TYPE_AUTONOMOUS_AI && m_allowedToIntervene;
    if (m_runCommand || autonomousDriving) ShowIntervention(m_interventionAmount);
}

/// @brief Intervenes the simulation if allowed to run the command
/// @param p_allowedActions The allowed black box actions, for determining whether or not the command may be ran
void IDecision::RunInterveneCommands(tAllowedActions p_allowedActions)
{
    m_allowedToIntervene = CanIntervene(p_allowedActions);
    m_runCommand = m_decisionMade && m_allowedToIntervene;
    if (m_runCommand && SMediator::GetInstance()->GetInterventionType() != INTERVENTION_TYPE_ONLY_SIGNALS) DoIntervention(m_interventionAmount);
}

/// @brief Sets the intervention amount and checks whether the threshold is reached
/// @param p_interventionAmount The intervention amount
void IDecision::SetInterventionAmount(float p_interventionAmount)
{
    m_interventionAmount = p_interventionAmount;
    m_decisionMade = ReachThreshold(p_interventionAmount);
}

/// @brief Gets the intervention amount
float IDecision::GetInterventionAmount() const
{
    return m_interventionAmount;
}

/// @brief Gets whether a decision has been made this time step
/// @return returns whether a decision has been made
bool IDecision::GetDecisionMade() const
{
    return m_decisionMade;
}

/// @brief Resets the current decision
void IDecision::Reset()
{
    m_interventionAmount = 0;
    m_decisionMade = false;
    m_allowedToIntervene = false;
    m_runCommand = false;
}