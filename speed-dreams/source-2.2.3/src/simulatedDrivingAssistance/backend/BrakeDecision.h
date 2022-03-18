#pragma once
#include "IDecision.h"

class BrakeDecision : public IDecision
{
public:
    float m_brakeAmount;

    void RunIndicateCommands() override;
    void RunInterveneCommands() override;
};