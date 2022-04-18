#pragma once
#include <fstream>

/// @brief A class that can record the input of a player for integration tests
class Recorder
{
public:
    Recorder(const std::string& p_dirName, const std::string& p_fileName, int p_paramAmount);
    ~Recorder();
    void WriteRecording(const float* p_input, const double p_currentTime, const bool p_compression);

    bool CheckSameInput(const float* p_input) const;

private:
    std::ofstream m_recordingFile;
    float* m_prevInput;
    int m_paramAmount;
};
