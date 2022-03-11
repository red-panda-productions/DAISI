#pragma once

class IDecision
{
public:
    virtual void GetIndicateCommands() = 0; //should be [ICommand]
    virtual void GetInterveneCommands() = 0; //should be [ICommand]
};