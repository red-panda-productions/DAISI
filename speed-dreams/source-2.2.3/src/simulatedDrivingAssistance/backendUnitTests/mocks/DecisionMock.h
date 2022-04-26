#pragma once

class DecisionMock : public IDecision
{
public:
    // @brief    increases the indicate integer variable by 1
    void RunIndicateCommands() override
    {
        indicate++;
    }

    // @brief    increases the intervene integer variable by 1
    void RunInterveneCommands() override
    {
        intervene++;
    }

    int indicate = 0; //int variable that checks if an indicator should be shown
    int intervene = 0; //int variable that checks if an intervention should happen
};