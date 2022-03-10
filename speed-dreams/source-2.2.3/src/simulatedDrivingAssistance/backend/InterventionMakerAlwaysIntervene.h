#pragma once

#include <iostream>
#include "InterventionMaker.h"

class InterventionMakerAlwaysIntervene : public InterventionMaker
{
    void MakeCommands() override
    {
        std::cout << "Make commands that always intervene" << std::endl;
    }
};