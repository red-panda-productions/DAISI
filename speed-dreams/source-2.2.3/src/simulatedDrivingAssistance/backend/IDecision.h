#pragma once

class IDecision
{
public:
    virtual void RunIndicateCommands() = 0; 
    virtual void RunInterveneCommands() = 0; 
};