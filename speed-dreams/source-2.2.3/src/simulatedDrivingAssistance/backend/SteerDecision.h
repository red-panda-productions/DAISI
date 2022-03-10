#pragma once
#include "IDecision.h"

class SteerDecision : public IDecision
{
public:
    float m_steerAmount;

    void GetIndicateCommands() override;
    void GetInterveneCommands() override;
};