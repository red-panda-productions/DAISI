#pragma once
#include "IDecision.h"

#define SDA_STEERING_THRESHOLD 0.04f

/// @brief Represents a steer decision that can be made by an AI
class SteerDecision : public IDecision
{
public:
    float SteerAmount = 0;

    void RunIndicateCommands() override;
    void RunInterveneCommands(tAllowedActions p_allowedActions) override;
};