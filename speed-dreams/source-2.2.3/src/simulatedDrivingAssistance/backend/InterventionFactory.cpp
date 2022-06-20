/*
 * This program has been developed by students from the bachelor Computer Science at
 * Utrecht University within the Software Project course.
 * © Copyright Utrecht University (Department of Information and Computing Sciences)
 */

#include "InterventionFactory.h"
#include "InterventionExecutorNoIntervention.h"
#include "InterventionExecutorAlwaysIntervene.h"
#include "InterventionExecutorAutonomousAI.h"
#include "InterventionExecutorIndication.h"
#include "InterventionExecutorPerformWhenNeeded.h"
#include <iostream>

#define CASE(type, class)   \
    case type:              \
    {                       \
        return new class(); \
    }

/// @brief			Creates the correct interventionExecutor based on the Intervention type
/// @param  p_type  The interventionType
/// @return			The InterventionExecutor
InterventionExecutor* InterventionFactory::CreateInterventionExecutor(InterventionType p_type)
{
    switch (p_type)
    {
        CASE(INTERVENTION_TYPE_AUTONOMOUS_AI, InterventionExecutorAutonomousAI)
        CASE(INTERVENTION_TYPE_COMPLETE_TAKEOVER, InterventionExecutorAlwaysIntervene)
        CASE(INTERVENTION_TYPE_ONLY_SIGNALS, InterventionExecutorIndication)
        CASE(INTERVENTION_TYPE_SHARED_CONTROL, InterventionExecutorPerformWhenNeeded)
        default:
            CASE(INTERVENTION_TYPE_NO_SIGNALS, InterventionExecutorNoIntervention)
    }
}
