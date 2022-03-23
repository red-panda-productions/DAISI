#pragma once
#include "IDecision.h"

/// @brief Represents a steer decision that can be made by an AI
class SteerDecision : public IDecision
{
public:
    float m_steerAmount;

    void RunIndicateCommands() override;
    void RunInterveneCommands() override;
};