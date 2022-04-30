#pragma once
#include <fstream>
#include <car.h>

#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING 1
#include <experimental/filesystem>

#define USER_INPUT_RECORDING_FILE_NAME "recording.txt"
#define DECISIONS_RECORDING_FILE_NAME  "decisions.txt"
#define CAR_SETTINGS_FILE_NAME         "car.xml"

/// @brief A class that can record the input of a player for integration tests
class Recorder
{
public:
    Recorder(const std::string& p_dirName, const std::string& p_fileName, int p_userParamAmount, int p_decisionParamAmount);
    ~Recorder();
    void WriteCar(const tCarElt* p_carElt);
    void WriteUserInput(const float* p_userInput, double p_timestamp, bool p_useCompression = false);
    void WriteDecisions(const float* p_decisions, unsigned long p_timestamp);

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
