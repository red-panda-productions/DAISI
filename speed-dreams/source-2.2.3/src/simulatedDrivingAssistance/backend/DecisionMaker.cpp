#include "DecisionMaker.h"
#include <iostream>

void DecisionMaker::ChangeSettings(INTERVENTION_TYPE type)
{
    m_interventionMaker = m_config.SetInterventionType(type);

    m_interventionMaker->MakeCommands();
}