#include "Recorder.h"
#include <iomanip>
#include <sstream>
#include <iostream>
#include <tgf.h>

///
/// @brief Constructor of Recording, 
/// creates a file with the current date and time,
/// opens file in binary and appending mode
/// records the start time of creation,
/// initializes the previous input with 2's since user input is between -1 an 1.
/// also creates a folder for the files
/// @author Sjoerd Schilder
///
Recorder::Recorder()
{
	// create directory
	std::string dirName = "..\\test_data\\user_recordings";
	if(!GfDirExists(dirName.c_str()))
		GfDirCreate(dirName.c_str());
	// get current date and time
	std::time_t t = std::time(nullptr);
	std::tm tm = *std::localtime(&t);
	std::stringstream buffer;
	buffer << std::put_time(&tm, "%Y%m%d-%H%M%S");
	// create file
	std::string fileName = dirName + "\\Record" + buffer.str() + ".txt";
	m_recordingFile.open(fileName, std::ios::binary | std::ios::app);
	// start clock
	m_startTime = clock();

	// initialize previous input with impossible values
	for (int i = 0; i < PARAMETERS; i++)
	{
		m_prevInput[i] = 2.0f; // 2.0f is impossible user input
	}

}

Recorder::~Recorder()
{

	m_recordingFile.close();
	delete m_prevInput;
}

///
/// @brief Writes a float* to the m_recordingFile, 
/// with the clock counting since construction of the recorder.
/// Only writes to the file if the input is different then the previous input.
/// @preconditions p_input must be PARAMETERS long (in this case 4)
/// @param p_input array of size PARAMETERS
/// @return void
/// @author Sjoerd Schilder
///
void Recorder::WriteRecording(float* p_input)
{
	// doesnt write if the input is the same as the previous time
	if (CheckSameInput(p_input)) return;
	clock_t clck;
	clck = clock() - m_startTime;
	m_recordingFile << (float)clck << " ";
	for (int i = 0; i < PARAMETERS; i++)
	{
		// update previous input
		m_prevInput[i] = p_input[i];
		// write to file
		m_recordingFile << p_input[i] << " ";
	}
	m_recordingFile << std::endl;
}

///
/// @brief Checks if the input is the same as the previous input
/// @preconditions p_input must be PARAMETERS long (in this case 4)
/// @param isSame boolean ref that becomes false if the input is differrent
/// @param p_input array of size PARAMETERS
/// @return void
/// @author Sjoerd Schilder
///
bool Recorder::CheckSameInput(float* p_input)
{
	for (int i = 0; i < PARAMETERS; i++)
	{
		if (p_input[i] != m_prevInput[i]) return false;
	}
	return true;
}
