#pragma once
#include "IDecision.h"

class BrakeDecision : public IDecision
{
public:
    float m_brakeAmount;

    void GetIndicateCommands() override;
    void GetInterveneCommands() override;
};