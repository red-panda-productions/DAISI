#include "DecisionMaker.h"

void DecisionMaker::ChangeSettings(INTERVENTION_TYPE p_type)
{
    m_interventionMaker = m_config.SetInterventionType(p_type);

    // this line is for debugging purposes only, should be removed
    m_interventionMaker->MakeCommands();
}