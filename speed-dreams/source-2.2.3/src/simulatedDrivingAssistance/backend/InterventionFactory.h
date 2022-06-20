/*
 * This program has been developed by students from the bachelor Computer Science at
 * Utrecht University within the Software Project course.
 * © Copyright Utrecht University (Department of Information and Computing Sciences)
 */

#pragma once
#include "InterventionExecutor.h"
#include "ConfigEnums.h"

/// @brief Creates executors according to the config file
class InterventionFactory
{
public:
    InterventionExecutor* CreateInterventionExecutor(InterventionType p_type);
};