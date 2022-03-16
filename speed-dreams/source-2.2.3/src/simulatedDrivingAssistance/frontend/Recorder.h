#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <ctime>

class Recorder
{
public:
	Recorder();
	void WriteRecording(float* p_input);

private:
	std::ofstream m_RecordingFile;
	std::string m_FileName;
};
