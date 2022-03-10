#pragma once

class InterventionMaker
{
public:
    // returns an array of commands based on the decisions made by the black box and the intervention type
    /* TODO: Change to [ICommand], include Decision as param (exists in branch: msgpack) */ virtual void MakeCommands() = 0;
};