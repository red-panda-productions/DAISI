#include "Recorder.h"
#include <iomanip>
#include <sstream>
#include <tgf.h>
#include "../rppUtils/RppUtils.hpp"

namespace filesystem = std::experimental::filesystem;

/// @brief Create a file to record the data to. Truncate if the file already exists.
/// @param p_recordingsFolder The folder to place the file in
/// @param p_decisionsRecordingFile  The stream to open the file on
/// @param p_fileName The filename of the file to open
void CreateRecordingFile(const std::experimental::filesystem::path& p_recordingsFolder, std::ofstream& p_decisionsRecordingFile, const char* p_fileName)
{
    // Open the files with truncate on, such that if the file was already in use any existing content will be discarded
    filesystem::path filePath = filesystem::path(p_recordingsFolder).append(p_fileName);
    std::cout << "Writing decisions to " << filePath << std::endl;
    p_decisionsRecordingFile.open(filePath, std::ios::binary | std::ios::trunc);
}

/// @brief  Constructor of Recording,
///	    	creates a file with the current date and time,
///	    	opens file in binary and appending mode
///	    	initializes the previous input with 2's since user input is between -1 an 1.
///	    	also creates a folder for the files.
/// @param p_dirName	        the name of the directory that needs to be placed in the SDA appdata directory.
/// @param p_fileNamePattern    the pattern for the output file name, as taken by std::put_time, without file extension
/// @param p_userParamAmount	the amount of parameters that are saved for every line of user inputs.
/// @param p_simulationDataParamAmount	the amount of parameters that are saved for every line of simulation data.
Recorder::Recorder(const std::string& p_dirName,
                   const std::string& p_fileNamePattern,
                   int p_userParamAmount,
                   int p_simulationDataParamAmount)
    : m_userParamAmount(p_userParamAmount), m_simulationDataParamAmount(p_simulationDataParamAmount)
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

    CreateRecordingFile(m_recordingDir, m_userInputRecordingFile, USER_INPUT_RECORDING_FILE_NAME);
    CreateRecordingFile(m_recordingDir, m_decisionsRecordingFile, DECISIONS_RECORDING_FILE_NAME);
    CreateRecordingFile(m_recordingDir, m_simulationDataRecordingFile, SIMULATION_DATA_RECORDING_FILE_NAME);

    // initialize previous input with impossible values. This ensures the first actual values are always written when compression is enabled.
    m_prevUserInput = new float[m_userParamAmount];
    for (int i = 0; i < m_userParamAmount; i++)
    {
        m_prevUserInput[i] = NAN;
    }

    m_prevSimulationData = new float[m_simulationDataParamAmount];
    for (int i = 0; i < m_simulationDataParamAmount; i++)
    {
        m_prevSimulationData[i] = NAN;
    }
}

Recorder::~Recorder()
{
    delete[] m_prevUserInput;
    delete[] m_prevSimulationData;
    m_userInputRecordingFile.close();
    m_decisionsRecordingFile.close();
    m_simulationDataRecordingFile.close();
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
    WriteRecording(p_userInput, p_timestamp, m_userInputRecordingFile, m_userParamAmount, p_useCompression, m_prevUserInput);
}

/// @brief Write simulation data to the output file
/// @param p_simulationData Simulation data to write, should be an array >= m_simulationDataParamAmount
/// @param p_timestamp Timestamp at which the simulation data occurred
/// @param p_useCompression Whether to use compression while writing
void Recorder::WriteSimulationData(const float* p_simulationData, const double p_timeStamp, const bool p_useCompression)
{
    WriteRecording(p_simulationData, p_timeStamp, m_simulationDataRecordingFile, m_simulationDataParamAmount, p_useCompression, m_prevSimulationData);
}

/// @brief Write decision data to the output file
/// @param p_userInput Decision data to write, should be an array >= m_decisionParamAmount
/// @param p_timestamp Timestamp at which the decision data occured
void Recorder::WriteDecisions(const DecisionTuple* p_decisions, const unsigned long p_timestamp)
{
    if (p_decisions == nullptr)
    {
        WriteRecording(nullptr, p_timestamp, m_decisionsRecordingFile, 0, false, nullptr);
        return;
    }

    float decisionValues[DECISION_RECORD_PARAM_AMOUNT] =
        {
            p_decisions->GetSteer(),
            p_decisions->GetAccel(),
            p_decisions->GetBrake(),
            static_cast<float>(p_decisions->GetGear())};
    WriteRecording(decisionValues, p_timestamp, m_decisionsRecordingFile, DECISION_RECORD_PARAM_AMOUNT, false, nullptr);
}

#define CREATE_WRITE_RECORDING_DEFINITION(type)                           \
    template void Recorder::WriteRecording<type>(const float* p_input,    \
                                                 type p_currentTime,      \
                                                 std::ofstream& p_file,   \
                                                 const int p_paramAmount, \
                                                 bool p_useCompression,   \
                                                 float* p_prevInput);

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
template <typename TIME>
void Recorder::WriteRecording(const float* p_input,
                              TIME p_currentTime,
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
    p_file << bits(p_currentTime);
    for (int i = 0; i < p_paramAmount; i++)
    {
        // update previous input
        if (p_prevInput)
        {
            p_prevInput[i] = p_input[i];
        }

        // write to file
        p_file << bits(p_input[i]);
    }
    p_file.flush();
}

CREATE_WRITE_RECORDING_DEFINITION(int)
CREATE_WRITE_RECORDING_DEFINITION(unsigned long)
CREATE_WRITE_RECORDING_DEFINITION(double)

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
