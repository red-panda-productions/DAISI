#pragma once
#include "IDecision.h"

/// @brief Represents a brake decision that can be made by an AI
class BrakeDecision : public IDecision
{
public:
    float m_brakeAmount = 0;

    void RunIndicateCommands() override;
    void RunInterveneCommands() override;
};