#include <iostream>
#include "DecisionTuple.h"

/// @brief Constructs a decision tuple
DecisionTuple::DecisionTuple()
{
    for (int i = 0; i < DECISIONS_COUNT; i++)
    {
        m_buffer[0] = nullptr;
    }
}

/// @brief        Gets all active decisions
/// @param  count Returns the amount of decisions
/// @return       The decisions
IDecision** DecisionTuple::GetActiveDecisions(int& p_count)
{
    p_count = 0;
    if (m_brakeDecision.GetDecisionMade())
    {
        m_buffer[p_count++] = &m_brakeDecision;
    }

    if (m_steerDecision.GetDecisionMade())
    {
        m_buffer[p_count++] = &m_steerDecision;
    }

    if (m_accelDecision.GetDecisionMade())
    {
        m_buffer[p_count++] = &m_accelDecision;
    }

    return m_buffer;
}

/// @brief               Sets the brake value of a decision
/// @param  p_brakeValue The value
void DecisionTuple::SetBrakeDecision(float p_brakeValue)
{
    m_brakeDecision.SetInterventionAmount(p_brakeValue);
}

/// @brief               Sets the steer value of a decision
/// @param  p_steerValue The value
void DecisionTuple::SetSteerDecision(float p_steerValue)
{
    m_steerDecision.SetInterventionAmount(p_steerValue);
}

void DecisionTuple::SetGearDecision(int p_gearValue)
{
    // TODO: When a GearDecision is created, set its value here
}

void DecisionTuple::SetAccelDecision(float p_accelValue)
{
    m_accelDecision.SetInterventionAmount(p_accelValue);
}

void DecisionTuple::SetLightsDecision(bool p_lightsValue)
{
    // TODO: When a LightsDecision is created, set its value here
}

/// @brief  Gets the brake value
/// @return The brake value
float DecisionTuple::GetBrakeAmount() const
{
    return m_brakeDecision.GetInterventionAmount();
}

/// @brief  Gets the steer value
/// @return The steer value
float DecisionTuple::GetSteerAmount() const
{
    return m_steerDecision.GetInterventionAmount();
}

int DecisionTuple::GetGearAmount() const
{
    // TODO: When a GearDecision is created, get its value here
    return 0;
}

float DecisionTuple::GetAccelAmount() const
{
    return m_accelDecision.GetInterventionAmount();
}

/// @brief Gets whether to turn the lights on or off
/// @return True if the headlights should be on, False if the headlights should be off
bool DecisionTuple::GetLightsAmount() const
{
    // TODO: When a LightsDecision is created, get its value here
    return false;
}

/// @brief Gets whether this tuple contains a brake decision
/// @return True if tuple contains a brake decision
bool DecisionTuple::ContainsBrake() const
{
    return m_brakeDecision.GetDecisionMade();
}

/// @brief Gets whether this tuple contains a steer decision
/// @return True if tuple contains a steer decision
bool DecisionTuple::ContainsSteer() const
{
    return m_steerDecision.GetDecisionMade();
}

/// @brief Gets whether this tuple contains a gear shift decision
/// @return True if tuple contains a gear shift decision
bool DecisionTuple::ContainsGear() const
{
    return false;
}

/// @brief Gets whether this tuple contains an acceleration decision
/// @return True if tuple contains an acceleration decision
bool DecisionTuple::ContainsAccel() const
{
    return m_accelDecision.GetDecisionMade();
}

/// @brief Gets whether this tuple contains a headlights decision
/// @return True if tuple contains a headlights decision
bool DecisionTuple::ContainsLights() const
{
    return false;
}

/// @brief Reset all decisions
void DecisionTuple::Reset()
{
    m_accelDecision.Reset();
    m_brakeDecision.Reset();
    m_steerDecision.Reset();
}
