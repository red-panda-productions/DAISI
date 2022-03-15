#pragma once
#include "BrakeDecision.h"
#include "SteerDecision.h"


#define DECISIONS_COUNT 2

struct DecisionTuple
{
public:
    DecisionTuple();

    IDecision** GetDecisions();


    BrakeDecision m_brakeDecision;
    SteerDecision m_steerDecision;

private:
    IDecision* buffer[2];
    
};