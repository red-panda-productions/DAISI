#pragma once
#include "BrakeDecision.h"
#include "SteerDecision.h"
#define DECISIONS_COUNT 2

/// @brief A tuple that contains all decisions that can be made by an AI
struct DecisionTuple
{
public:
    DecisionTuple();

    IDecision** GetActiveDecisions(int& p_count);

    void SetBrake(float p_brakeValue);

    void SetSteer(float p_steerValue);

    float GetBrake() const;

    float GetSteer() const;


private:
    IDecision* buffer[DECISIONS_COUNT];

    BrakeDecision m_brakeDecision;
    bool m_brakeActive = false;
    SteerDecision m_steerDecision;
    bool m_steerActive = false;

};