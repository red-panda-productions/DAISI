#pragma once
#include "InterventionMaker.h"
#include "ConfigEnums.h"

class InterventionFactory
{
public:
    InterventionMaker* CreateInterventionMaker(INTERVENTION_TYPE p_type);
};