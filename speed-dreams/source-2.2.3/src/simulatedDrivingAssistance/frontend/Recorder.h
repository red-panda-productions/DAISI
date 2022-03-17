#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <algorithm>

class Recorder
{
public:
	Recorder();
	void WriteRecording(float* p_input);

private:
	std::ofstream m_recordingFile;
	std::string m_fileName;
};
