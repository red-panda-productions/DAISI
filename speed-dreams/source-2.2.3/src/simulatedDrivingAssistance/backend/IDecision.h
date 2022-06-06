#pragma once
#include "ConfigEnums.h"

/// @brief An interface for a decision that can be made by an AI
class IDecision
{
public:
    /// @brief Can show an indicator
    void RunIndicateCommands();

    /// @brief Intervenes the simulation if allowed to run the command
    void RunInterveneCommands(tAllowedActions p_allowedActions);

    /// @brief Sets the intervention amount and checks whether the threshold is reached
    void SetInterventionAmount(float p_interventionAmount);

    /// @brief Gets the intervention amount
    float GetInterventionAmount() const;

    /// @brief Gets whether a decision has been made
    bool GetDecisionMade() const;

    /// @brief resets the current decision
    void Reset();

    virtual ~IDecision() = default;

protected:
    virtual void ShowIntervention(float p_interventionAmount) = 0;
    virtual bool ReachThreshold(float p_interventionAmount) = 0;
    virtual bool CanIntervene(tAllowedActions p_allowedActions) = 0;
    virtual void DoIntervention(float p_interventionAmount) = 0;

private:
    float m_interventionAmount = 0;
    bool m_decisionMade = false;
    bool m_allowedToIntervene = false;
    bool m_runCommand = false;
};