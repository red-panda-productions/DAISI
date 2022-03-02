#pragma once
#include "IInterventionMaker.h"
#include "ConfigEnums.h"

class InterventionFactory
{
public:
    void CreateInterventionMaker(IInterventionMaker& p_interventionMaker, INTERVENTION_TYPE p_type);
};