#include "Recorder.h"
#include <iomanip>
#include <sstream>
#include <iostream>

std::string fileName;
#define MAX_PARAMETERS 4 // steerCmd, accelCmd, brakeCmd, clutchCmd.
Recorder::Recorder()
{
	std::time_t t = std::time(nullptr);
	std::tm tm = *std::localtime(&t);
	std::stringstream buffer;
	buffer << std::put_time(&tm, "%Y%m%d-%H%M%S");

	fileName = "..\\src\\simulatedDrivingAssistance\\data\\frontendRecordings\\Record"+ buffer.str() + ".txt";
	m_recordingFile.open(fileName, std::ios::binary | std::ios::app);



}

Recorder::~Recorder()
{
	m_recordingFile.close();
}





void Recorder::WriteRecording(float* p_Input)
{
	if (!m_startTime)
	{
		m_startTime = clock();
	}
	clock_t clck;
	clck = clock() - m_startTime;
	//if (m_prevInput &&
	//	//m_prevInput+1 &&
	//	//m_prevInput+2 &&
	//	//m_prevInput+3 &&
	//	p_Input[0] == m_prevInput[0] &&
	//	p_Input[1] == m_prevInput[1] &&
	//	p_Input[2] == m_prevInput[2] &&
	//	p_Input[3] == m_prevInput[3])
	//{
	//	return;
	//}
	m_recordingFile << (float)clck << " ";
	for (int i = 0; i < MAX_PARAMETERS; i++)
	{
		m_recordingFile << p_Input[i] << " ";
	}
	m_recordingFile<< std::endl;
	m_prevInput = p_Input;
}