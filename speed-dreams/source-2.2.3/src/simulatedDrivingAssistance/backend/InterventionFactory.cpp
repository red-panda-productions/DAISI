#include "InterventionFactory.h"
#include "InterventionMakerNoIntervention.h"
#include <iostream>

void InterventionFactory::CreateInterventionMaker(IInterventionMaker& p_interventionMaker, INTERVENTION_TYPE p_type)
{
   switch(p_type)
   {
       case INTERVENTION_TYPE_NO_INTERVENTION: std::cout << "NO INTERVENTION" << std::endl;
   }
}