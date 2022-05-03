#pragma once
#include <fstream>
#include <car.h>

#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING 1
#include <experimental/filesystem>

#include "DecisionTuple.h"

#define USER_INPUT_RECORDING_FILE_NAME      "recording.txt"
#define DECISIONS_RECORDING_FILE_NAME       "decisions.txt"
#define SIMULATION_DATA_RECORDING_FILE_NAME "simulation_data.txt"
#define CAR_SETTINGS_FILE_NAME              "car.xml"

#define DECISION_RECORD_PARAM_AMOUNT 4

/// @brief A class that can record the input of a player for integration tests
class Recorder
{
public:
    Recorder(const std::string& p_dirName, const std::string& p_fileName, int p_userParamAmount, int p_simulationDataParamAmount);
    ~Recorder();
    void WriteCar(const tCarElt* p_carElt);
    void WriteUserInput(const float* p_userInput, double p_timestamp, bool p_useCompression = false);
    void WriteDecisions(const DecisionTuple* p_decisions, unsigned long p_timestamp);
    void WriteSimulationData(const float* p_simulationData, double p_timeStamp, bool p_useCompression = false);

    template <typename TIME>
    void WriteRecording(const float* p_input, TIME p_currentTime, std::ofstream& p_file, int p_paramAmount, bool p_useCompression, float* p_prevInput);

private:
    static bool CheckSameInput(const float* p_input, const float* p_prevInput, int p_paramAmount);

    std::ofstream m_userInputRecordingFile;
    std::ofstream m_decisionsRecordingFile;
    std::ofstream m_simulationDataRecordingFile;
    std::experimental::filesystem::path m_recordingDir;
    float* m_prevUserInput;
    float* m_prevSimulationData;
    int m_userParamAmount;
    int m_simulationDataParamAmount;
};
