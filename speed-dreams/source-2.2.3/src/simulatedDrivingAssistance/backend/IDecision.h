#pragma once

/// @brief An interface for a decision that can be made by an AI
class IDecision
{
public:
	/// @brief Can show an indicator
	virtual void RunIndicateCommands() = 0;

	/// @brief Can intervene the simulation
	virtual void RunInterveneCommands() = 0; 
};