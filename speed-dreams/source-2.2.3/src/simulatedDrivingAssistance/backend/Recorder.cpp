#include "Recorder.h"
#include <iomanip>
#include <sstream>
#include <tgf.h>
#include "../rppUtils/RppUtils.hpp"

/// @brief  Constructor of Recording,
///	    	creates a file with the current date and time,
///	    	opens file in binary and appending mode
///	    	initializes the previous input with 2's since user input is between -1 an 1.
///	    	also creates a folder for the files.
/// @param p_dirName	        the name of the directory that needs to be placed in the SDA appdata directory.
/// @param p_fileNamePattern    the pattern for the output file name, as taken by std::put_time, without file extension
/// @param p_paramAmount	the amount of parameters that you want to save per line.
Recorder::Recorder(const std::string& p_dirName, const std::string& p_fileNamePattern, const int p_paramAmount)
{
    std::experimental::filesystem::path sdaFolder;
    if (!GetSdaFolder(sdaFolder)) return;
    auto recordingsFolder = sdaFolder.append(p_dirName);
    std::string recordingsFolderString = recordingsFolder.string();
    if (!GfDirExists(recordingsFolderString.c_str()))
    {
        GfDirCreate(recordingsFolderString.c_str());
    }

    // create a new recording directory with current timestamp
    std::time_t t = std::time(nullptr);
    std::tm tm = *std::localtime(&t);
    std::stringstream buffer;
    buffer << std::put_time(&tm, p_fileNamePattern.c_str());
    recordingsFolder.append(buffer.str());
    m_recordingDir = std::experimental::filesystem::path(recordingsFolder);
    create_directories(m_recordingDir);

    // Open the file with truncate on, such that if the file was already in use any existing content will be discarded
    std::experimental::filesystem::path path = recordingsFolder.append(USER_INPUT_RECORDING_FILE_NAME);
    m_recordingFile.open(path, std::ios::binary | std::ios::trunc);
    std::cout << "Writing recording file to " << path << std::endl;
    m_recordingFile << std::fixed << std::setprecision(20);

    m_paramAmount = p_paramAmount;
    // initialize previous input with impossible values. This ensures the first actual values are always written when compression is enabled.
    m_prevInput = new float[p_paramAmount];
    for (int i = 0; i < m_paramAmount; i++)
    {
        m_prevInput[i] = 2.0f;  // 2.0f is impossible user input
    }
}

Recorder::~Recorder()
{
    delete[] m_prevInput;
    m_recordingFile.close();
}

/// @brief Write the settings of the car being recorded to the recording.
/// @param p_carElt The handle to the car settings.
void Recorder::WriteCar(const tCarElt* p_carElt) {
    GfParmWriteFile(std::experimental::filesystem::path(m_recordingDir).append(CAR_SETTINGS_FILE_NAME).string().c_str(), p_carElt->_carHandle, p_carElt->info.name);
}

/// @brief  Writes a float array to the m_recordingFile,
///	    	with the current time of the simulation.
///	    	Can do compression if p_compression is true,
///	    	then it only writes to the file if the input
///	    	is different then the previous input.
/// @param p_input          array of size m_parameterAmount
/// @param p_currentTime    the current time of the simulation
/// @param p_compression    boolean value if compression is done
void Recorder::WriteRecording(const float* p_input, const double p_currentTime, const bool p_compression)
{
	// doesn't write if the input is the same as the previous time
	// if p_compression is true
	if (p_compression && CheckSameInput(p_input)) return;
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

/// @brief				Checks if the input is the same as the previous input
/// @param p_input		array of size m_paramAmount
bool Recorder::CheckSameInput(const float* p_input) const
{
    for (int i = 0; i < m_paramAmount; i++)
    {
        if (p_input[i] != m_prevInput[i]) return false;
    }
    return true;
}