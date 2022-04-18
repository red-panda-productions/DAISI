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
    if (m_brakeActive)
    {
        m_buffer[p_count++] = &m_brakeDecision;
    }

    if (m_steerActive)
    {
        m_buffer[p_count++] = &m_steerDecision;
    }

    if (m_accelActive)
    {
        m_buffer[p_count++] = &m_accelDecision;
    }

    return m_buffer;
}

/// @brief               Sets the brake value of a decision
/// @param  p_brakeValue The value
void DecisionTuple::SetBrake(float p_brakeValue)
{
    m_brakeDecision.m_brakeAmount = p_brakeValue;
    m_brakeActive = true;
}

/// @brief               Sets the steer value of a decision
/// @param  p_steerValue The value
void DecisionTuple::SetSteer(float p_steerValue)
{
    m_steerDecision.m_steerAmount = p_steerValue;
    m_steerActive = true;
}

void DecisionTuple::SetGear(int p_gearValue)
{
}

void DecisionTuple::SetAccel(float p_accelValue)
{
    m_accelDecision.AccelAmount = p_accelValue;
    m_accelActive = true;
}

/// @brief  Gets the brake value
/// @return The brake value
float DecisionTuple::GetBrake() const
{
    return m_brakeDecision.m_brakeAmount;
}

/// @brief  Gets the steer value
/// @return The steer value
float DecisionTuple::GetSteer() const
{
    return m_steerDecision.m_steerAmount;
}

float DecisionTuple::GetGear() const
{
    return 0;
}

float DecisionTuple::GetAccel() const
{
    return m_accelDecision.AccelAmount;
}
