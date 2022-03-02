#include "Config.h"

void Config::SetInterventionType(IInterventionMaker& p_interventionMaker, INTERVENTION_TYPE p_type)
{
    m_interventionType = p_type;
    interventionFactory.CreateInterventionMaker(p_interventionMaker, p_type);
}

INTERVENTION_TYPE Config::GetInterventionType() const
{
    return m_interventionType;
}