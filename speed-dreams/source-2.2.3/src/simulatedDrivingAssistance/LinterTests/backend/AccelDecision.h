#pragma once
#include "IDecision.h"

class AccelDecision : public IDecision
{
public:
    float AccelAmount = 0;

    void RunIndicateCommands() override;
    void RunInterveneCommands() override;
};