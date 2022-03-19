#pragma once
#include "InterventionExecutor.h"
#include "ConfigEnums.h"

/// @brief Creates executors according to the config file
class InterventionFactory
{
public:
    InterventionExecutor* CreateInterventionExecutor(INTERVENTION_TYPE p_type);
};