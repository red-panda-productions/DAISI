#pragma once
#include <fstream>


class Recorder
{
public:
	Recorder();
	~Recorder();
	void WriteRecording(float* p_input);

private:
	std::ofstream m_recordingFile;
	float* m_prevInput;
	std::clock_t m_startTime;
};
