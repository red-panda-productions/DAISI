#pragma once

#include <iostream>
#include "InterventionMaker.h"

class InterventionMakerPerformWhenNeeded : public InterventionMaker
{
    void MakeCommands() override
    {
        std::cout << "Make commands that perform when needed" << std::endl;
    }
};