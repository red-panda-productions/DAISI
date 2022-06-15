#pragma once
#include "BrakeDecision.h"
#include "SteerDecision.h"
#include "AccelDecision.h"

#define DECISIONS_COUNT 3

/// @brief A tuple that contains all decisions that can be made by an AI
struct DecisionTuple
{
public:
    DecisionTuple();

    Decision** GetActiveDecisions(int& p_count);

    void SetBrakeDecision(float p_brakeValue);
    void SetSteerDecision(float p_steerValue);
    void SetGearDecision(int p_gearValue);
    void SetAccelDecision(float p_accelValue);
    void SetLightsDecision(bool p_lightsValue);

    float GetBrakeAmount() const;
    float GetSteerAmount() const;
    int GetGearAmount() const;
    float GetAccelAmount() const;
    bool GetLightsAmount() const;

    bool ContainsBrake() const;
    bool ContainsSteer() const;
    bool ContainsGear() const;
    bool ContainsAccel() const;
    bool ContainsLights() const;

    void Reset();

private:
    Decision* m_buffer[DECISIONS_COUNT] = {};

    BrakeDecision m_brakeDecision;
    SteerDecision m_steerDecision;
    AccelDecision m_accelDecision;
};