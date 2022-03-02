#include "DecisionMaker.h"

void DecisionMaker::ChangeSettings(INTERVENTION_TYPE type)
{
    m_config.SetInterventionType(*m_interventionMaker, type);
}