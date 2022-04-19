#pragma once
#include "BrakeDecision.h"
#include "SteerDecision.h"
#include "AccelDecision.h"

#define DECISIONS_COUNT 3

/// @brief A tuple that contains all decisions that can be made by an AI
//template <class Mediator>
struct DecisionTuple
{
public:
    DecisionTuple();

    IDecision** GetActiveDecisions(int& p_count);

    void SetBrake(float p_brakeValue);

    void SetSteer(float p_steerValue);

    void SetGear(int p_gearValue);

    void SetAccel(float p_accelValue);

    float GetBrake() const;

    float GetSteer() const;

    float GetGear() const;

    float GetAccel() const;

    bool GetLights() const;

    bool ContainsBrake() const;
    bool ContainsSteer() const;
    bool ContainsGear() const;
    bool ContainsAccel() const;
    bool ContainsLights() const;

private:
    IDecision* m_buffer[DECISIONS_COUNT] = {};

    BrakeDecision m_brakeDecision;
    bool m_brakeActive = false;
    SteerDecision m_steerDecision;
    bool m_steerActive = false;
    AccelDecision m_accelDecision;
    bool m_accelActive = false;
};