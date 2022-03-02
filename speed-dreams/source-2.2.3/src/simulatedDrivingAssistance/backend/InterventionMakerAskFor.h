#pragma once

#include <iostream>
#include "InterventionMaker.h"

class InterventionMakerAskFor : public InterventionMaker
{
    void MakeCommands() override
    {
        std::cout << "Make commands that ask to be done" << std::endl;
    }
};