/*
 * This program has been developed by students from the bachelor Computer Science at
 * Utrecht University within the Software Project course.
 * © Copyright Utrecht University (Department of Information and Computing Sciences)
 */

#pragma once
#include "Decision.h"

/// @brief Represents a steer decision that can be made by an AI
class SteerDecision : public Decision
{
protected:
    void ShowIntervention(float p_interventionAmount) override;
    bool ReachThreshold(float p_interventionAmount) override;
    bool CanIntervene(tAllowedActions p_allowedActions) override;
    void DoIntervention(float p_interventionAmount) override;
};