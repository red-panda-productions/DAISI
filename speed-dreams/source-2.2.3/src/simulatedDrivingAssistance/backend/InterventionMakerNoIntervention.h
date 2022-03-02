#pragma once
#include "IInterventionMaker.h"

class InterventionMakerNoIntervention : IInterventionMaker
{
    void MakeCommands() override;
};