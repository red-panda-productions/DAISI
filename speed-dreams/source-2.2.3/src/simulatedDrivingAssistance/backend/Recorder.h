#pragma once
#include <fstream>
#include <car.h>
#include "ConfigEnums.h"

#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING 1
#include <experimental/filesystem>

namespace filesystem = std::experimental::filesystem;

#include "DecisionTuple.h"

#define RECORDING_EXTENSION ".bin"

#define USER_INPUT_RECORDING_FILE_NAME      "recording" RECORDING_EXTENSION
#define DECISIONS_RECORDING_FILE_NAME       "decisions" RECORDING_EXTENSION
#define CAR_SETTINGS_FILE_NAME              "car.xml"
#define SIMULATION_DATA_RECORDING_FILE_NAME "simulation_data" RECORDING_EXTENSION
#define RUN_SETTINGS_FILE_NAME              "settings.xml"

#define PATH_PARTICIPANT_CONTROL "participant_control"
#define PATH_TRACK               "environment/track"
#define PATH_INTERVENTION_TYPE   "intervention_type"
#define PATH_INDICATORS          "indicators"
#define PATH_VERSION             "version"
#define PATH_MAX_TIME            "max_time"
#define PATH_ALLOWED_ACTION      "allowed_actions"

#define KEY_INDICATOR_AUDIO "audio"
#define KEY_INDICATOR_ICON  "icon"
#define KEY_INDICATOR_TEXT  "text"

#define KEY_SELECTED "selected"
#define KEY_FILENAME "filename"
#define KEY_CATEGORY "category"
#define KEY_NAME     "name"
#define KEY_VERSION  "version"

#define KEY_PARTICIPANT_CONTROL_CONTROL_STEERING            "control_steering"
#define KEY_PARTICIPANT_CONTROL_CONTROL_GAS                 "control_gas"
#define KEY_PARTICIPANT_CONTROL_CONTROL_BRAKE               "control_brake"
#define KEY_PARTICIPANT_CONTROL_CONTROL_INTERVENTION_TOGGLE "control_intervention_toggle"
#define KEY_PARTICIPANT_CONTROL_FORCE_FEEDBACK              "force_feedback"
#define KEY_PARTICIPANT_CONTROL_RECORD_SESSION              "record_session"
#define KEY_PARTICIPANT_CONTROL_BB_RECORD_SESSION           "bb_record_session"

#define KEY_MAX_TIME "max_time"

#define KEY_ALLOWED_ACTION_STEER      "steer"
#define KEY_ALLOWED_ACTION_ACCELERATE "accelerate"
#define KEY_ALLOWED_ACTION_BRAKE      "brake"

#define CURRENT_RECORDER_VERSION 4
#define DEFAULT_MAX_TIME         10

#define DECISION_RECORD_PARAM_AMOUNT 4

/// @brief A class that can record the input of a player for integration tests
class Recorder
{
public:
    Recorder(const std::string& p_dirName, const std::string& p_fileName, int p_userParamAmount, int p_simulationDataParamAmount);
    ~Recorder();
    void WriteRunSettings(const tCarElt* p_carElt, const tTrack* p_track, const tIndicator& p_indicators, const InterventionType& p_interventionType, const tParticipantControl& p_participantControl, int p_maxTime, const tAllowedActions& p_allowedActions);
    void WriteUserInput(const float* p_userInput, double p_timestamp, bool p_useCompression = false);
    void WriteDecisions(const DecisionTuple* p_decisions, unsigned long p_timestamp);
    void WriteSimulationData(const float* p_simulationData, double p_timeStamp, bool p_useCompression = false);

    template <typename TIME>
    void WriteRecording(const float* p_input, TIME p_currentTime, std::ofstream& p_file, int p_paramAmount, bool p_useCompression, float* p_prevInput);

    static bool ValidateAndUpdateRecording(const filesystem::path& p_recordingFolder);

private:
    static bool CheckSameInput(const float* p_input, const float* p_prevInput, int p_paramAmount);

    std::ofstream m_userInputRecordingFile;
    std::ofstream m_decisionsRecordingFile;
    std::ofstream m_simulationDataRecordingFile;
    filesystem::path m_recordingDir;
    float* m_prevUserInput;
    float* m_prevSimulationData;
    int m_userParamAmount;
    int m_simulationDataParamAmount;
};
