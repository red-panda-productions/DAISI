#pragma once
#include "BrakeDecision.h"
#include "SteerDecision.h"


#define DECISIONS_COUNT 2

struct DecisionTuple
{
public:
    DecisionTuple()
    {
        buffer[0] = &m_brakeDecision;
        buffer[1] = &m_steerDecision;
    }

    IDecision** GetDecisions()
    {
        return buffer;
    }

    BrakeDecision m_brakeDecision;
    SteerDecision m_steerDecision;

private:
    IDecision* buffer[2];
    
};