#pragma once
#include "IDecision.h"

/// @brief Represents a accelerate decision that can be made by an AI
class AccelDecision : public IDecision
{
public:
    float AccelAmount = 0;

    void RunIndicateCommands() override;
    void RunInterveneCommands() override;
};