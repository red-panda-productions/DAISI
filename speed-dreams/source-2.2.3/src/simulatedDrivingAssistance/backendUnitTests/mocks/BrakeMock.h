#pragma once

class BrakeMock : public IDecision
{
public:
    void RunIndicateCommands() override
    {
        indicate++;
    }

    void RunInterveneCommands() override
    {
        intervene++;
    }

    int indicate = 0;
    int intervene = 0;
};