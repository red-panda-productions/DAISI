#include "Recorder.h"
#include <iomanip>
#include <sstream>
#include <tgf.h>

///
/// @brief Constructor of Recording, 
/// creates a file with the current date and time,
/// opens file in binary and appending mode
/// initializes the previous input with 2's since user input is between -1 an 1.
/// also creates a folder for the files.
///	@param p_dirName the name of the directory that needs to be placed in the test_data directory.
///	@param p_fileName the name of the file placed in the directory,
///	the current date will be added to this name
///	@param p_paramAmount the amount of parameters that you want to save per line.
/// @author Sjoerd Schilder
///
Recorder::Recorder(const std::string& p_dirName, const std::string& p_fileName, const int p_paramAmount)
{
	// create directory if it doesn't exist
	std::string dirPath = "..\\test_data\\" + p_dirName;
	if(!GfDirExists(dirPath.c_str())) 
	{
		GfDirCreate(dirPath.c_str());
	}
	
	// create and open new recording file with current timestamp
	std::time_t t = std::time(nullptr);
	std::tm tm = *std::localtime(&t);
	std::stringstream buffer;
	buffer << std::put_time(&tm, "%Y%m%d-%H%M%S");
	std::string path = dirPath + "\\" + p_fileName + buffer.str() + ".txt";
	m_recordingFile.open(path, std::ios::binary | std::ios::app);

	m_paramAmount = p_paramAmount;
	// initialize previous input with impossible values
	for(int i = 0; i < m_paramAmount; i++)
	{
		m_prevInput[i] = 2.0f; // 2.0f is impossible user input
	}

}

Recorder::~Recorder()
{
	m_recordingFile.close();
}

///
/// @brief Writes a float* to the m_recordingFile, 
/// with the current time of the simulation.
/// Only writes to the file if the input is different then the previous input.
/// @preconditions p_input must be PARAMETERS long (in this case 4)
/// @param p_input array of size PARAMETERS
///	@param p_currentTime the current time of the simulation
/// @author Sjoerd Schilder
///
void Recorder::WriteRecording(const float* p_input, const double p_currentTime)
{
	// doesn't write if the input is the same as the previous time
	if (CheckSameInput(p_input)) return;
	m_recordingFile << p_currentTime << " ";
	for (int i = 0; i < m_paramAmount; i++)
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
/// @param p_input array of size PARAMETERS
/// @author Sjoerd Schilder
///
bool Recorder::CheckSameInput(const float* p_input) const
{
	for (int i = 0; i < m_paramAmount; i++)
	{
		if (p_input[i] != m_prevInput[i]) return false;
	}
	return true;
}