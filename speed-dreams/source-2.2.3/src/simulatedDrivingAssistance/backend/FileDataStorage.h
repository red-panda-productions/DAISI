#pragma once

#include <fstream>
#include "BlackBoxData.h"
#include "ConfigEnums.h"
#include "DecisionTuple.h"
#include <ctime>

/// @brief               A class that can store data to a file
/// @tparam BlackBoxData The data that needs to be stored
class FileDataStorage
{
private:
    /// @brief Boolean array to determine what to save and what not to save. Uses indices as in ConfigEnums.h
    tDataToStore m_saveSettings;
    /// @brief Output filestream to write data to, should be initialized through @link FileDataStorage::Initialize
    std::ofstream m_outputStream = {};
    /// @brief Integer that determines the compression rate
    int m_compressionRate;
    /// @brief Integer that determines the current step of the compression
    unsigned long m_compressionStep;

    float m_totalPosX = 0, m_totalPosY = 0, m_totalPosZ = 0, m_totalPosAx = 0, m_totalPosAy = 0, m_totalPosAz = 0;
    float m_totalMovVelX = 0, m_totalMovAccX = 0;
    // float m_gearValues[];
    // float m_steerValues[];
    // float m_brakeValues[];
    // float m_accelValues[];
    // float m_clutchValues[];

public:
    std::experimental::filesystem::path Initialize(
        tDataToStore p_saveSettings,
        const std::string& p_fileName,
        const std::string& p_userId,
        const std::time_t& p_trialStartTime,
        const std::string& p_blackboxFilename,
        const std::string& p_blackboxName,
        const std::time_t& p_blackboxTime,
        const std::string& p_environmentFilename,
        const std::string& p_environmentName,
        int p_environmentVersion,
        InterventionType p_interventionType);

    void SetCompressionRate(int p_compressionRate);

    int GetCompressionRate();

    void AddForAveraging(float& p_total, float p_value);

    float GetAverage(float& p_total);

    void AddToArray(float p_values[], float p_value, unsigned long p_compressionStep);

    float GetMedian(float p_values[]);

    float GetLeastCommon(float p_values[]);

    void Shutdown();

    void Save(tCarElt* p_car, tSituation* p_situation, unsigned long p_timestamp);

    void SaveDecisions(DecisionTuple& p_decisions);

    ~FileDataStorage() = default;
};

/// @brief Standard implementation of the file data storage
#define SFileDataStorage FileDataStorage<BlackBoxData>
