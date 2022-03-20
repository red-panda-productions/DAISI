#include "DecisionTuple.h"

DecisionTuple::DecisionTuple()
{
    for(int i = 0; i < DECISIONS_COUNT; i++)
    {
        buffer[0] = nullptr;
    }
}

IDecision** DecisionTuple::GetActiveDecisions(int& count)
{
    count = 0;
    if(m_brakeActive)
    {
        buffer[count++] = &m_brakeDecision;
    }

    if(m_steerActive)
    {
        buffer[count++] = &m_steerDecision;
    }

    return buffer;
}

void DecisionTuple::SetBrake(float p_brakeValue)
{
	m_brakeDecision.m_brakeAmount = p_brakeValue;
	m_brakeActive = true;
}

void DecisionTuple::SetSteer(float p_steerValue)
{
	m_steerDecision.m_steerAmount = p_steerValue;
	m_steerActive = true;
}

float DecisionTuple::GetBrake()
{
	return m_brakeDecision.m_brakeAmount;
}

float DecisionTuple::GetSteer()
{
	return m_steerDecision.m_steerAmount;
}
