#pragma once
#include "InterventionExecutor.h"
#include "ConfigEnums.h"

class InterventionFactory
{
public:
    InterventionExecutor* CreateInterventionExecutor(INTERVENTION_TYPE p_type);
};