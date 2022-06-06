#pragma once
#include "IDecision.h"

/// @brief Represents a steer decision that can be made by an AI
class SteerDecision : public IDecision
{
public:
    void ShowIntervention(float p_interventionAmount) override;
    bool ReachThreshold(float p_interventionAmount) override;
    bool CanIntervene(tAllowedActions p_allowedActions) override;
    void DoIntervention(float p_interventionAmount) override;
};