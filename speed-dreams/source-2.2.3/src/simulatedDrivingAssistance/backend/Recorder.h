#pragma once
#include <fstream>
#define PARAMETERS 4 // steerCmd, accelCmd, brakeCmd, clutchCmd.

class Recorder
{

public:

	Recorder(const std::string& p_dirName, const std::string& p_fileName, int p_paramAmount);
	~Recorder();
	void WriteRecording(const float* p_input, const double p_currentTime);

	bool CheckSameInput(const float* p_input) const;

private:
	std::ofstream m_recordingFile;
	float m_prevInput[PARAMETERS];
	int m_paramAmount;
};
