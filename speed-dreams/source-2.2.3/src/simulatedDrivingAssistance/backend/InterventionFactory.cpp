#include "InterventionFactory.h"
#include "InterventionMakerNoIntervention.h"
#include "InterventionMakerAlwaysIntervene.h"
#include "InterventionMakerAskFor.h"
#include "InterventionMakerIndication.h"
#include "InterventionMakerPerformWhenNeeded.h"

#include <iostream>

// THE STD::COUT LINE CAN BE REMOVED AFTER THE DEMO (IF WE DO ONE)
#define CASE(type, class) case type: { \
    std::cout << #type << std::endl; \
    return new class();}

InterventionMaker* InterventionFactory::CreateInterventionMaker(INTERVENTION_TYPE p_type)
{
    switch(p_type)
   {
       CASE(INTERVENTION_TYPE_ALWAYS_INTERVENE, InterventionMakerAlwaysIntervene)
       CASE(INTERVENTION_TYPE_ASK_FOR, InterventionMakerAskFor)
       CASE(INTERVENTION_TYPE_INDICATION, InterventionMakerIndication)
       CASE(INTERVENTION_TYPE_PERFORM_WHEN_NEEDED, InterventionMakerPerformWhenNeeded)
       default:
       CASE(INTERVENTION_TYPE_NO_INTERVENTION, InterventionMakerNoIntervention)
   }
}