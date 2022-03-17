#include "DecisionTuple.h"

DecisionTuple::DecisionTuple()
{
    buffer[0] = &m_brakeDecision;
    buffer[1] = &m_steerDecision;
}

IDecision** DecisionTuple::GetDecisions()
{
    return buffer;
}

