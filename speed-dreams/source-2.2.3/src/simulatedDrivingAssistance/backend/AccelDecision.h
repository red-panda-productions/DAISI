#pragma once
#include "IDecision.h"

class AccelDecision : public IDecision
{
public:
    float AccelAmount;

    void RunIndicateCommands() override;
    void RunInterveneCommands() override;
};