#pragma once

#include <iostream>
#include "InterventionMaker.h"

class InterventionMakerIndication : public InterventionMaker
{
    void MakeCommands() override
    {
        std::cout << "Make commands that show indication" << std::endl;
    }
};