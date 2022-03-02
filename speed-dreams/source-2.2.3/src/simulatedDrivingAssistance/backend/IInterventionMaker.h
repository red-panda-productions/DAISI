#pragma once

class IInterventionMaker
{
public:
    /* TODO: Change to [ICommand], include Decision as param (exists in branch: msgpack) */ virtual void MakeCommands() = 0;
};