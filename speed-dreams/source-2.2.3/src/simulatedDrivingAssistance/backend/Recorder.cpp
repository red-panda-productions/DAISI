#include "Recorder.h"
#include <iomanip>
#include <sstream>
#include <iostream>

/**
* @brief Constructor of Recording, 
* creates a file with the current date and time,
* opens file in binary and appending mode
* records the start time of creation,
* initializes the previous input with 2's since user input is between -1 an 1.
* @author Sjoerd Schilder
*/
Recorder::Recorder()
{
	std::time_t t = std::time(nullptr);
	std::tm tm = *std::localtime(&t);
	std::stringstream buffer;
	buffer << std::put_time(&tm, "%Y%m%d-%H%M%S");

	std:: string fileName = "..\\src\\simulatedDrivingAssistance\\data\\userRecordings\\Record"+ buffer.str() + ".txt";
	m_recordingFile.open(fileName, std::ios::binary | std::ios::app);
	m_startTime = clock();

	for (int i = 0; i < PARAMETERS; i++)
	{
		m_prevInput[i] = 2.0f; // 2.0f is impossible user input
	}

}

Recorder::~Recorder()
{
	m_recordingFile.close();
}

/**
* @brief Writes a float* to the m_recordingFile, 
* with the clock counting since construction of the recorder.
* Only writes to the file if the input is different then the previous input.
* @preconditions p_input must be PARAMETERS long (in this case 4)
* @param p_input array of size PARAMETERS
* @return void
* @author Sjoerd Schilder
*/
void Recorder::WriteRecording(float* p_input)
{
	bool isSame = true;
	CheckSameInput(isSame, p_input);
	// doesnt write if the input is the same as the previous time
	if (isSame)
	{
		return;
	}

	WriteInputToFile(p_input);

	// saves the current input as previous input
	for (int i = 0; i < PARAMETERS; i++)
	{
		m_prevInput[i] = p_input[i];
	}
}

/**
* @brief Does the actual writing to the file,
* with the clock counting since construction of the recorder,
* spaces in between and a new line at the end
* @preconditions p_input must be PARAMETERS long (in this case 4)
* @param p_input array of size PARAMETERS
* @return void
* @author Sjoerd Schilder
*/
void Recorder::WriteInputToFile(float* p_input)
{
	clock_t clck;
	clck = clock() - m_startTime;
	m_recordingFile << (float)clck << " ";
	for (int i = 0; i < PARAMETERS; i++)
	{
		m_recordingFile << p_input[i] << " ";
	}
	m_recordingFile << std::endl;
}

/**
* @brief Checks if the input is the same as the previous input
* @preconditions p_input must be PARAMETERS long (in this case 4)
* @param isSame boolean ref that becomes false if the input is differrent
* @param p_input array of size PARAMETERS
* @return void
* @author Sjoerd Schilder
*/
void Recorder::CheckSameInput(bool& isSame, float* p_input)
{
	for (int i = 0; i < PARAMETERS && isSame; i++)
	{
		if (p_input[i] != m_prevInput[i])
		{
			isSame = false;
		}
	}
}
