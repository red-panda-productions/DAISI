#pragma once
#include "InterventionExecutor.h"
#include "ConfigEnums.h"

/// @brief Creates executors according to the config file
class InterventionFactory
{
public:
    InterventionExecutor* CreateInterventionExecutor(InterventionType p_type);
};