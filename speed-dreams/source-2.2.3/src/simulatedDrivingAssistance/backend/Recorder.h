#pragma once
#include <fstream>
#include <car.h>
#include "ConfigEnums.h"

#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING 1
#include <experimental/filesystem>

#include "DecisionTuple.h"

#define USER_INPUT_RECORDING_FILE_NAME "recording.txt"
#define DECISIONS_RECORDING_FILE_NAME  "decisions.txt"
#define CAR_SETTINGS_FILE_NAME         "car.xml"
#define RUN_SETTINGS_FILE_NAME         "settings.xml"

#define PATH_PARTICIPANT_CONTROL "participant_control"
#define PATH_TRACK "environment/track"
#define PATH_INTERVENTION_TYPE "intervention_type"
#define PATH_INDICATORS "indicators"

#define KEY_INDICATOR_AUDIO "audio"
#define KEY_INDICATOR_ICON "icon"
#define KEY_INDICATOR_TEXT "text"

#define KEY_SELECTED "selected"
#define KEY_FILENAME "filename"

#define KEY_PARTICIPANT_CONTROL_CONTROL_GAS "control_gas"
#define KEY_PARTICIPANT_CONTROL_CONTROL_INTERVENTION_TOGGLE "control_intervention_toggle"
#define KEY_PARTICIPANT_CONTROL_CONTROL_STEERING "control_steering"
#define KEY_PARTICIPANT_CONTROL_FORCE_FEEDBACK "force_feedback"
#define KEY_PARTICIPANT_CONTROL_RECORD_SESSION "record_session"
#define KEY_PARTICIPANT_CONTROL_BB_RECORD_SESSION "bb_record_session"

#define DECISION_RECORD_PARAM_AMOUNT 4

/// @brief A class that can record the input of a player for integration tests
class Recorder
{
public:
    Recorder(const std::string& p_dirName, const std::string& p_fileName, int p_userParamAmount, int p_decisionParamAmount);
    ~Recorder();
    void WriteRunSettings(const tCarElt* p_carElt, const tTrack* p_track, const tIndicator& p_indicators,const InterventionType& p_interventionType, const tParticipantControl& p_participantControl);
    void WriteUserInput(const float* p_userInput, double p_timestamp, bool p_useCompression = false);
    void WriteDecisions(const DecisionTuple* p_decisions, unsigned long p_timestamp);

    template <typename TIME>
    void WriteRecording(const float* p_input, TIME p_currentTime, std::ofstream& p_file, int p_paramAmount, bool p_useCompression, float* p_prevInput);

private:
    static bool CheckSameInput(const float* p_input, const float* p_prevInput, int p_paramAmount);

    std::ofstream m_userInputRecordingFile;
    std::ofstream m_decisionsRecordingFile;
    std::experimental::filesystem::path m_recordingDir;
    float* m_prevUserInput;
    int m_userParamAmount;
    int m_decisionParamAmount;
};
