#pragma once
#include "IDecision.h"

class SteerDecision : public IDecision
{
public:
    float m_steerAmount;

    void RunIndicateCommands() override;
    void RunInterveneCommands() override;
};