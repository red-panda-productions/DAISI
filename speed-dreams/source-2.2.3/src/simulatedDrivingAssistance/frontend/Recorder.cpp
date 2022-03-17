#include "Recorder.h"
#include <iomanip>
#include <sstream>

#define MAX_PARAMETERS 4 // steerCmd, accelCmd, brakeCmd, clutchCmd.
Recorder::Recorder()
{
	std::time_t t = std::time(nullptr);
	std::tm tm = *std::localtime(&t);
	std::stringstream buffer;
	buffer << std::put_time(&tm, "%Y%m%d-%H%M%S");

	m_fileName = "..\\src\\simulatedDrivingAssistance\\data\\frontendRecordings\\Record"+ buffer.str() + ".txt";
	std::ofstream recordingFile(m_fileName);

	recordingFile.close();

}





void Recorder::WriteRecording(float* p_Input)
{

	m_recordingFile.open(m_fileName);
	m_recordingFile << p_Input[0] << " " << p_Input[1] << " " << p_Input[2] << " " << p_Input[3] << std::endl;

}