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
/// @param p_userParamAmount	the amount of parameters that are saved for every line of user inputs.
/// @param p_decisionParamAmount	the amount of parameters that are saved for every line of decision parameters.
Recorder::Recorder(const std::string& p_dirName,
                   const std::string& p_fileNamePattern,
                   int p_userParamAmount,
                   int p_decisionParamAmount)
    : m_userParamAmount(p_userParamAmount), m_decisionParamAmount(p_decisionParamAmount)
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

    // Open the files with truncate on, such that if the file was already in use any existing content will be discarded
    std::experimental::filesystem::path
        path = std::experimental::filesystem::path(recordingsFolder).append(USER_INPUT_RECORDING_FILE_NAME);
    std::cout << "Writing user input to " << path << std::endl;
    m_userInputRecordingFile.open(path, std::ios::binary | std::ios::trunc);
    m_userInputRecordingFile << std::fixed << std::setprecision(20);

    path = std::experimental::filesystem::path(recordingsFolder).append(DECISIONS_RECORDING_FILE_NAME);
    std::cout << "Writing decisions to " << path << std::endl;
    m_decisionsRecordingFile.open(path, std::ios::binary | std::ios::trunc);
    m_decisionsRecordingFile << std::fixed << std::setprecision(20);

    // initialize previous input with impossible values. This ensures the first actual values are always written when compression is enabled.
    m_prevUserInput = new float[m_userParamAmount];
    for (int i = 0; i < m_userParamAmount; i++)
    {
        m_prevUserInput[i] = 2.0f;  // 2.0f is impossible user input
    }
}

Recorder::~Recorder()
{
    delete[] m_prevUserInput;
    m_userInputRecordingFile.close();
    m_decisionsRecordingFile.close();
}

/// @brief Write the settings of the car being recorded to the recording.
/// @param p_carElt The handle to the car settings.
void Recorder::WriteCar(const tCarElt* p_carElt)
{
    GfParmWriteFile(std::experimental::filesystem::path(m_recordingDir).append(CAR_SETTINGS_FILE_NAME).string().c_str(),
                    p_carElt->_carHandle,
                    p_carElt->info.name);
}

/// @brief Write user input to the output file
/// @param p_userInput User input to write, should be an array >= m_userParamAmount
/// @param p_timestamp Timestamp at which the user input occurred
/// @param p_useCompression Whether to use compression while writing
void Recorder::WriteUserInput(const float* p_userInput, const double p_timestamp, const bool p_useCompression)
{
    WriteRecording(p_userInput,
                   p_timestamp,
                   m_userInputRecordingFile,
                   m_userParamAmount,
                   p_useCompression,
                   m_prevUserInput);
}

/// @brief Write decision data to the output file
/// @param p_userInput Decision data to write, should be an array >= m_decisionParamAmount
/// @param p_timestamp Timestamp at which the decision data occured
void Recorder::WriteDecisions(const float* p_decisions, const unsigned long p_timestamp)
{
    WriteRecording(p_decisions, p_timestamp, m_decisionsRecordingFile, m_decisionParamAmount, false, nullptr);
}

/// @brief  Writes a float array to the m_recordingFile,
///	    	with the current time of the simulation.
///	    	Can do compression if p_compression is true,
///	    	then it only writes to the file if the input
///	    	is different then the previous input.
/// @param p_input          array of size >= m_parameterAmount
/// @param p_currentTime    the current time of the simulation
/// @param p_file           the file to write to
/// @param p_paramAmount    Amount of parameters to write, should be <= size of p_input
/// @param p_useCompression boolean value if compression is done
/// @param p_prevInput      the previous input, used for compression, length should be p_paramAmount and not nullptr if p_useCompression is true.
void Recorder::WriteRecording(const float* p_input,
                              const double p_currentTime,
                              std::ofstream& p_file,
                              const int p_paramAmount,
                              bool p_useCompression,
                              float* p_prevInput)
{
    if (p_useCompression && p_prevInput == nullptr)
    {
        throw std::exception("Compression is enabled but no previous input is given");
    }

    // doesn't write if the input is the same as the previous time
    // if p_compression is true
    if (p_useCompression && CheckSameInput(p_input, p_prevInput, p_paramAmount)) return;
    p_file << p_currentTime << " ";
    for (int i = 0; i < p_paramAmount; i++)
    {
        // update previous input
        if (p_prevInput)
        {
            p_prevInput[i] = p_input[i];
        }

        // write to file
        p_file << p_input[i] << " ";
    }
    p_file << std::endl;
}

/// @brief				    Checks if the input is the same as the previous input
/// @param p_input		    The new input that should be recorded.
/// @param p_prevInput      The old input from the previous line recorded.
/// @param p_paramAmount	The amount of parameters in the input.
/// @return				    True if the input is the same as the previous input, false otherwise.
bool Recorder::CheckSameInput(const float* p_input, const float* p_prevInput, int p_paramAmount)
{
    for (int i = 0; i < p_paramAmount; i++)
    {
        if (p_input[i] != p_prevInput[i]) return false;
    }
    return true;
}
