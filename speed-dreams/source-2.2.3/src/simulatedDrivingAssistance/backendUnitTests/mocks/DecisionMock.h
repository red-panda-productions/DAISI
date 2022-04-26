#pragma once
#include "IDecision.h"

class DecisionMock : public IDecision
{
public:
    // @brief    increases the indicate integer variable by 1
    void RunIndicateCommands() override
    {
        Indicate++;
    }

    // @brief    increases the intervene integer variable by 1
    void RunInterveneCommands() override
    {
        Intervene++;
    }

    int Indicate = 0;   // int variable that checks if an indicator should be shown
    int Intervene = 0;  // int variable that checks if an intervention should happen
};