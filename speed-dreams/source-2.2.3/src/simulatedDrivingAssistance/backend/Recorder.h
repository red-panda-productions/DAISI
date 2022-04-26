#pragma once
#include <fstream>
#include <car.h>

#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING 1
#include <experimental/filesystem>

#define USER_INPUT_RECORDING_FILE_NAME "recording.txt"
#define CAR_SETTINGS_FILE_NAME "car.xml"

/// @brief A class that can record the input of a player for integration tests
class Recorder
{
public:
    Recorder(const std::string& p_dirName, const std::string& p_fileName, int p_paramAmount);
    ~Recorder();
    void WriteCar(const tCarElt* p_carElt);
    void WriteRecording(const float* p_input, const double p_currentTime, const bool p_compression);

    bool CheckSameInput(const float* p_input) const;

private:
    std::ofstream m_recordingFile;
    std::experimental::filesystem::path m_recordingDir;
    float* m_prevInput;
    int m_paramAmount;
};
