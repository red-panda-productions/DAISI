#pragma once
#include "BrakeDecision.h"
#include "SteerDecision.h"


#define DECISIONS_COUNT 2

struct DecisionTuple
{
public:
    DecisionTuple();

    IDecision** GetActiveDecisions(int& count);

    void SetBrake(float p_brakeValue);

    void SetSteer(float p_steerValue);

    float GetBrake();

    float GetSteer();


private:
    IDecision* buffer[DECISIONS_COUNT];

    BrakeDecision m_brakeDecision;
    bool m_brakeActive = false;
    SteerDecision m_steerDecision;
    bool m_steerActive = false;

};