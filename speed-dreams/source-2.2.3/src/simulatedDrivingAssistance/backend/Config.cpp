#include "Config.h"

void Config::SetInterventionType(INTERVENTION_TYPE p_type)
{
    m_interventionType = p_type;
}

INTERVENTION_TYPE Config::GetInterventionType() const
{
    return m_interventionType;
}