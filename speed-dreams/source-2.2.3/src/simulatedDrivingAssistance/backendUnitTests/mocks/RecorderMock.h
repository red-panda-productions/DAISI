/*
 * This program has been developed by students from the bachelor Computer Science at
 * Utrecht University within the Software Project course.
 * © Copyright Utrecht University (Department of Information and Computing Sciences)
 */

#pragma once
#include <fstream>
#include <car.h>
#include "DecisionTuple.h"
#include "FileSystem.hpp"

#define USER_INPUT_RECORDING_FILE_NAME "recording.txt"
#define DECISIONS_RECORDING_FILE_NAME  "decisions.txt"
#define CAR_SETTINGS_FILE_NAME         "car.xml"

/// @brief A class that can record the input of a player for integration tests
class RecorderMock
{
public:
    void WriteCar(const tCarElt* p_carElt);
    void WriteUserInput(const float* p_userInput, double p_timestamp, bool p_useCompression = false);
    void WriteDecisions(const DecisionTuple* p_decisions, unsigned long p_timestamp)
    {
        CurrentDecisions = *p_decisions;
        CurrentTimestamp = p_timestamp;
    };
    static void WriteRecording(const float* p_input, double p_currentTime, std::ofstream& p_file, int p_paramAmount, bool p_useCompression, float* p_prevInput);

    DecisionTuple CurrentDecisions;
    unsigned long CurrentTimestamp;

private:
    static bool CheckSameInput(const float* p_input, const float* p_prevInput, int p_paramAmount);

    std::ofstream m_userInputRecordingFile;
    std::ofstream m_decisionsRecordingFile;
    filesystem::path m_recordingDir;
    float* m_prevUserInput;
    int m_userParamAmount;
    int m_decisionParamAmount;
};
