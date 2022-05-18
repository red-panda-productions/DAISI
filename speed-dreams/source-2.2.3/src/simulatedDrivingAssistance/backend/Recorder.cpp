#include "Recorder.h"
#include <iomanip>
#include <sstream>
#include <tgf.h>
#include "../rppUtils/RppUtils.hpp"

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

/// @brief Write the settings of the run being recorded to the recording folder.
/// @param p_carElt               The car being used.
/// @param p_track                The track that's being driven on
/// @param p_indicators           The indicator settings
/// @param p_interventionType     The intervention type settings
/// @param p_participantControl   The participant control settings
/// @param p_maxTime             The maximum time for the trial
/// @param p_allowedActions       The allowed black box actions settings
void Recorder::WriteRunSettings(const tCarElt* p_carElt, const tTrack* p_track, const tIndicator& p_indicators, const InterventionType& p_interventionType, const tParticipantControl& p_participantControl, const int p_maxTime, const tAllowedActions& p_allowedActions)
{
    using std::experimental::filesystem::path;
    GfParmWriteFile(path(m_recordingDir).append(CAR_SETTINGS_FILE_NAME).string().c_str(),
                    p_carElt->_carHandle,
                    p_carElt->info.name);

    void* settingsFileHandle = GfParmReadFile(path(m_recordingDir).append(RUN_SETTINGS_FILE_NAME).string().c_str(), GFPARM_RMODE_CREAT, true);

    GfParmSetStr(settingsFileHandle, PATH_INDICATORS, KEY_INDICATOR_AUDIO, BoolToString(p_indicators.Audio));
    GfParmSetStr(settingsFileHandle, PATH_INDICATORS, KEY_INDICATOR_ICON, BoolToString(p_indicators.Icon));
    GfParmSetStr(settingsFileHandle, PATH_INDICATORS, KEY_INDICATOR_TEXT, BoolToString(p_indicators.Text));

    GfParmSetNum(settingsFileHandle, PATH_INTERVENTION_TYPE, KEY_SELECTED, nullptr, static_cast<float>(p_interventionType));

    GfParmSetStr(settingsFileHandle, PATH_TRACK, KEY_CATEGORY, p_track->category);
    GfParmSetStr(settingsFileHandle, PATH_TRACK, KEY_NAME, p_track->name);

    GfParmSetStr(settingsFileHandle, PATH_PARTICIPANT_CONTROL, KEY_PARTICIPANT_CONTROL_CONTROL_STEERING, BoolToString(p_participantControl.ControlSteer));
    GfParmSetStr(settingsFileHandle, PATH_PARTICIPANT_CONTROL, KEY_PARTICIPANT_CONTROL_CONTROL_GAS, BoolToString(p_participantControl.ControlAccel));
    GfParmSetStr(settingsFileHandle, PATH_PARTICIPANT_CONTROL, KEY_PARTICIPANT_CONTROL_CONTROL_BRAKE, BoolToString(p_participantControl.ControlBrake));
    GfParmSetStr(settingsFileHandle, PATH_PARTICIPANT_CONTROL, KEY_PARTICIPANT_CONTROL_CONTROL_INTERVENTION_TOGGLE, BoolToString(p_participantControl.ControlInterventionToggle));
    GfParmSetStr(settingsFileHandle, PATH_PARTICIPANT_CONTROL, KEY_PARTICIPANT_CONTROL_FORCE_FEEDBACK, BoolToString(p_participantControl.ForceFeedback));

    GfParmSetNum(settingsFileHandle, PATH_MAX_TIME, KEY_MAX_TIME, nullptr, (tdble)p_maxTime);

    GfParmSetStr(settingsFileHandle, PATH_ALLOWED_ACTION, KEY_ALLOWED_ACTION_STEER, BoolToString(p_allowedActions.Steer));
    GfParmSetStr(settingsFileHandle, PATH_ALLOWED_ACTION, KEY_ALLOWED_ACTION_ACCELERATE, BoolToString(p_allowedActions.Accelerate));
    GfParmSetStr(settingsFileHandle, PATH_ALLOWED_ACTION, KEY_ALLOWED_ACTION_BRAKE, BoolToString(p_allowedActions.Brake));

    GfParmSetNum(settingsFileHandle, PATH_VERSION, KEY_VERSION, nullptr, CURRENT_RECORDER_VERSION);

    GfParmWriteFile(nullptr, settingsFileHandle, "Run Settings");
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

/// @brief Update a v0 recording to a v1 recording. This means:
///  - Convert track filename to category and name
///  - Change file extensions from txt to bin
/// @param p_settingsHandle Handle to the run settings file
/// @param p_userRecordingFile Path to the user recordings file
/// @param p_decisionsRecordingFile Path to the decision recordings file
/// @param p_simulationFile Path to the simulation file
/// @return true if updating was successful
bool UpdateV0RecorderToV1(void* p_settingsHandle, filesystem::path& p_userRecordingFile, filesystem::path& p_decisionsRecordingFile, filesystem::path& p_simulationFile)
{
    const char* trackFileName = GfParmGetStr(p_settingsHandle, PATH_TRACK, KEY_FILENAME, nullptr);

    if (trackFileName == nullptr) return false;

    void* trackHandle = GfParmReadFile(trackFileName, 0, true);

    const char* category = strdup(GfParmGetStr(trackHandle, "Header", "category", nullptr));
    const char* name = strdup(GfParmGetStr(trackHandle, "Header", "name", nullptr));

    GfParmReleaseHandle(trackHandle);

    if (category == nullptr || name == nullptr)
    {
        free((void*)category);
        free((void*)name);
        return false;
    }

    GfParmSetStr(p_settingsHandle, PATH_TRACK, KEY_CATEGORY, category);
    GfParmSetStr(p_settingsHandle, PATH_TRACK, KEY_NAME, name);
    GfParmRemove(p_settingsHandle, PATH_TRACK, KEY_FILENAME);
    GfParmRemove(p_settingsHandle, PATH_PARTICIPANT_CONTROL, KEY_PARTICIPANT_CONTROL_RECORD_SESSION);
    GfParmRemove(p_settingsHandle, PATH_PARTICIPANT_CONTROL, KEY_PARTICIPANT_CONTROL_BB_RECORD_SESSION);

    filesystem::path fixedUserRecordingFile = filesystem::path(p_userRecordingFile).replace_extension(RECORDING_EXTENSION);
    filesystem::path fixedDecisionsRecordingFile = filesystem::path(p_decisionsRecordingFile).replace_extension(RECORDING_EXTENSION);
    filesystem::path fixedSimulationFile = filesystem::path(p_simulationFile).replace_extension(RECORDING_EXTENSION);

    filesystem::rename(p_userRecordingFile, fixedUserRecordingFile);
    filesystem::rename(p_decisionsRecordingFile, fixedDecisionsRecordingFile);
    filesystem::rename(p_simulationFile, fixedSimulationFile);

    p_userRecordingFile = fixedUserRecordingFile;
    p_decisionsRecordingFile = fixedDecisionsRecordingFile;
    p_simulationFile = fixedSimulationFile;

    return true;
}

/// @brief Update a v1 recording to a v2 recording. This means:
///  - Updating the replay driver to work with the max time
/// @param p_settingsHandle Handle to the run settings file
void UpdateV1RecorderToV2(void* p_settingsHandle)
{
    GfParmSetNum(p_settingsHandle, PATH_MAX_TIME, KEY_MAX_TIME, nullptr, DEFAULT_MAX_TIME);
}

/// @brief Update a v2 recording to a v3 recording. This means:
///  - Adding allowed black box actions
/// @param p_settingsHandle Handle to the run settings file
void UpdateV2RecorderToV3(void* p_settingsHandle)
{
    // All previous recording had all actions allowed
    GfParmSetStr(p_settingsHandle, PATH_ALLOWED_ACTION, KEY_ALLOWED_ACTION_STEER, BoolToString(true));
    GfParmSetStr(p_settingsHandle, PATH_ALLOWED_ACTION, KEY_ALLOWED_ACTION_ACCELERATE, BoolToString(true));
    GfParmSetStr(p_settingsHandle, PATH_ALLOWED_ACTION, KEY_ALLOWED_ACTION_BRAKE, BoolToString(true));
}

/// @brief Update a v3 recording to a v4 recording. This means:
///  - Adding an additional partipant control options to the settings.              
/// @param p_settingsHandle Handle to the run settings file
void UpdateV3RecorderToV4(void* p_settingsHandle)
{
    GfParmSetStr(p_settingsHandle, PATH_PARTICIPANT_CONTROL, KEY_PARTICIPANT_CONTROL_CONTROL_BRAKE, BoolToString(true));
}

/// @brief Validate a recording, and update it if it is an older version
/// @param p_recordingFolder Folder of the recording to validate and update
/// @return true if the recording contains all files and could be updated
bool Recorder::ValidateAndUpdateRecording(const filesystem::path& p_recordingFolder)
{
    // Make sure the settings file exists
    const filesystem::path settingsFile = filesystem::path(p_recordingFolder).append(RUN_SETTINGS_FILE_NAME);

    if (!exists(settingsFile))
    {
        return false;
    }

    void* settingsHandle = GfParmReadFile(settingsFile.string().c_str(), 0, true);

    // If it cannot be parsed the recording is invalid
    if (settingsHandle == nullptr)
    {
        return false;
    }

    // Get the version so the paths can be changed, since v0 paths end in txt and v1+ paths end in bin
    int version = static_cast<int>(GfParmGetNum(settingsHandle, PATH_VERSION, KEY_VERSION, nullptr, 0));

    const filesystem::path carSettingsFile = filesystem::path(p_recordingFolder).append(CAR_SETTINGS_FILE_NAME);
    filesystem::path decisionsRecordingFile = filesystem::path(p_recordingFolder).append(DECISIONS_RECORDING_FILE_NAME);
    filesystem::path userRecordingFile = filesystem::path(p_recordingFolder).append(USER_INPUT_RECORDING_FILE_NAME);
    filesystem::path simulationFile = filesystem::path(p_recordingFolder).append(SIMULATION_DATA_RECORDING_FILE_NAME);

    // Version 0 recording, so use txt paths
    if (version == 0)
    {
        decisionsRecordingFile.replace_extension("txt");
        userRecordingFile.replace_extension("txt");
        simulationFile.replace_extension("txt");
    }

    // Make sure all recordings exists
    if (!exists(carSettingsFile) || !exists(settingsFile) || !exists(decisionsRecordingFile) || !exists(userRecordingFile) || !exists(simulationFile))
    {
        return false;
    }

    // Update version 0 to version 1 recording
    if (version == 0)
    {
        if (!UpdateV0RecorderToV1(settingsHandle, userRecordingFile, decisionsRecordingFile, simulationFile))
        {
            GfParmReleaseHandle(settingsHandle);
            return false;
        }
        version++;
    }

    // Update version 1 to version 2 recording
    if (version == 1)
    {
        UpdateV1RecorderToV2(settingsHandle);
        version++;
    }

    // Update version 2 to version 3 recording
    if (version == 2)
    {
        UpdateV2RecorderToV3(settingsHandle);
        version++;
    }

    // Update from version 3 to version 4 recording
    if (version == 3)
    {
        UpdateV3RecorderToV4(settingsHandle);
        version++;
    }

    // Set the recording to the latest version and save it
    GfParmSetNum(settingsHandle, PATH_VERSION, KEY_VERSION, nullptr, CURRENT_RECORDER_VERSION);

    GfParmWriteFile(settingsFile.string().c_str(), settingsHandle, "Run Settings");
    GfParmReleaseHandle(settingsHandle);
    return true;
}