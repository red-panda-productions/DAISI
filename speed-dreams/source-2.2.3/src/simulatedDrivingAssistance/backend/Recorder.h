#pragma once
#include <fstream>
#define PARAMETERS 4 // steerCmd, accelCmd, brakeCmd, clutchCmd.

class Recorder
{

public:

	Recorder();
	~Recorder();
	void WriteRecording(float* p_input);

	bool CheckSameInput(float* p_input);

private:
	std::ofstream m_recordingFile;
	float m_prevInput[PARAMETERS];
	std::clock_t m_startTime;
};
