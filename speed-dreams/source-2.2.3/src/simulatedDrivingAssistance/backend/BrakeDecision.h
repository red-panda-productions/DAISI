#pragma once
#include "IDecision.h"

#define BRAKE_THRESHOLD 0.9f

/// @brief Represents a brake decision that can be made by an AI
class BrakeDecision : public IDecision
{
public:
    float BrakeAmount = 0;

    void RunIndicateCommands() override;
    void RunInterveneCommands(tAllowedActions p_allowedActions) override;
};