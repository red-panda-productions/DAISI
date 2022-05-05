#pragma once
#include "IDecision.h"

#define ACCEL_THRESHOLD 0.9f

class AccelDecision : public IDecision
{
public:
    float AccelAmount = 0;

    void RunIndicateCommands() override;
    void RunInterveneCommands() override;
};