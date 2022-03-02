#pragma once
#include "InterventionMaker.h"
#include "ConfigEnums.h"

class InterventionFactory
{
public:
    // returns the correct intervention maker based on p_type
    InterventionMaker* CreateInterventionMaker(INTERVENTION_TYPE p_type);
};