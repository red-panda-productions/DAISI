#pragma once

class InterventionMaker
{
public:
    /* TODO: Change to [ICommand], include Decision as param (exists in branch: msgpack) */ virtual void MakeCommands() = 0;
};