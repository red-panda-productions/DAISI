#include "InterventionFactory.h"
#include "InterventionExecutorNoIntervention.h"
#include "InterventionExecutorAlwaysIntervene.h"
#include "InterventionExecutorAskFor.h"
#include "InterventionExecutorIndication.h"
#include "InterventionExecutorPerformWhenNeeded.h"
#include <iostream>

// THE STD::COUT LINE CAN BE REMOVED AFTER THE DEMO (IF WE DO ONE)
#define CASE(type, class) case type: { \
    std::cout << #type << std::endl; \
    return new class();}

/// @brief			Creates the correct interventionExecutor based on the Intervention type
/// @param  p_type  The interventionType
/// @return			The InterventionExecutor
InterventionExecutor* InterventionFactory::CreateInterventionExecutor(InterventionType p_type)
{
	switch(p_type)
	{
	   CASE(INTERVENTION_TYPE_COMPLETE_TAKEOVER, InterventionExecutorAlwaysIntervene)
	   CASE(INTERVENTION_TYPE_ASK_FOR, InterventionExecutorAskFor)
	   CASE(INTERVENTION_TYPE_ONLY_SIGNALS, InterventionExecutorIndication)
	   CASE(INTERVENTION_TYPE_SHARED_CONTROL, InterventionExecutorPerformWhenNeeded)
	   default:
	   CASE(INTERVENTION_TYPE_NO_SIGNALS, InterventionExecutorNoIntervention)
	}
}

