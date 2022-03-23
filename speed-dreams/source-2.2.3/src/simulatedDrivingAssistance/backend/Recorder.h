#pragma once
#include <fstream>
#define PARAMETERS 4 // steerCmd, accelCmd, brakeCmd, clutchCmd.

/// @brief A class that can record the input of a player for integration tests
class Recorder
{

public:

	Recorder();
	~Recorder();
	void WriteRecording(float* p_input, double currentTime);

	bool CheckSameInput(float* p_input);

private:
	std::ofstream m_recordingFile;
	float m_prevInput[PARAMETERS];
};
