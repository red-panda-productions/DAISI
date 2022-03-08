#pragma once
#include "BrakeDecision.h"
#include "SteerDecision.h"

struct DecisionTuple
{
public:
    BrakeDecision brakeDecision;
    SteerDecision steerDecision;

    DecisionTuple()
    {
        buffer[0] = &brakeDecision;
    }

    IDecision** GetDecisions()
    {
        return buffer;
    }

private:
    IDecision* buffer[2];
};