#pragma once
#include "IDecision.h"

/// @brief Represents a brake decision that can be made by an AI
class BrakeDecision : public IDecision
{
public:
    void ShowIntervention(float p_interventionAmount) override;
    bool ReachThreshold(float p_interventionAmount) override;
    bool CanIntervene(tAllowedActions p_allowedActions) override;
    void DoIntervention(float p_interventionAmount) override;
};