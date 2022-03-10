#pragma once
#include "BrakeDecision.h"
#include "SteerDecision.h"

struct DecisionTuple
{
public:
    BrakeDecision m_brakeDecision;
    SteerDecision m_steerDecision;

    DecisionTuple()
    {
        buffer[0] = &m_brakeDecision;
        buffer[1] = &m_steerDecision;
    }

    IDecision** GetDecisions()
    {
        return buffer;
    }

private:
    IDecision* buffer[2];
};