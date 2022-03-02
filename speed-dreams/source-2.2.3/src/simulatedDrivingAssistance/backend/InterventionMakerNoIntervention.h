#pragma once

#include <iostream>
#include "InterventionMaker.h"

class InterventionMakerNoIntervention : public InterventionMaker
{
    void MakeCommands() override
    {
        std::cout << "Make no commands" << std::endl;
    }
};