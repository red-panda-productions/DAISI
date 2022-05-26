#pragma once
#include <fstream>
#include "BlackBoxData.h"
#include "ConfigEnums.h"
#include "DecisionTuple.h"
#include <ctime>
#include "../rppUtils/Random.hpp"

#include <experimental/filesystem>

/// @brief the max size of the array in which data is stored
#define COMPRESSION_LIMIT 50

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
    int m_compressionRate = 1;
    /// @brief Integer that determines the current step of the compression
    int m_compressionStep = 0;

    float m_totalPosX = 0, m_totalPosY = 0, m_totalPosZ = 0, m_totalPosAx = 0, m_totalPosAy = 0, m_totalPosAz = 0;
    float m_totalMovVelX = 0, m_totalMovAccX = 0;
    int m_gearValues[COMPRESSION_LIMIT];
    float m_steerValues[COMPRESSION_LIMIT], m_brakeValues[COMPRESSION_LIMIT], m_accelValues[COMPRESSION_LIMIT], m_clutchValues[COMPRESSION_LIMIT];
    float m_steerDecision[COMPRESSION_LIMIT], m_brakeDecision[COMPRESSION_LIMIT], m_accelDecision[COMPRESSION_LIMIT];
    int m_gearDecision[COMPRESSION_LIMIT], m_lightDecision[COMPRESSION_LIMIT];

    Random m_random;

    void SaveCarData(tCarElt* p_car);
    void SaveHumanData(tCarElt* p_car);
    void SaveInterventionData(DecisionTuple& p_decisions);

    void WriteCarData();
    void WriteHumanData();
    void WriteInterventionData();

    void GetMedianUtil(float* p_values, int p_start, int p_end, int p_middle, float& p_startPartition, float& p_endPartition);
    int RandomPartition(float* p_values, int p_start, int p_end);
    int Partition(float* p_values, int p_start, int p_end);

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

    int GetCompressionRate() const;

    void AddForAveraging(float& p_total, float p_value);

    float GetAverage(float& p_total) const;

    template <typename TNumber>
    void AddToArray(TNumber* p_values, TNumber p_value, int p_compressionStep) const;

    template <typename TNumber>
    void WriteDecision(TNumber p_decision, const std::string& p_decisionType, bool& p_decisionMade);

    template <typename TNumber>
    void SaveDecision(bool p_decisionMade, TNumber p_value, TNumber* p_values, int p_compressionStep);

    float GetMedian(float* p_values);

    int GetLeastCommon(int* p_values) const;

    void Shutdown();

    void Save(tCarElt* p_car, tSituation* p_situation, DecisionTuple& p_decisions, unsigned long p_timestamp);

    ~FileDataStorage() = default;
};

/// @brief Standard implementation of the file data storage
#define SFileDataStorage FileDataStorage<BlackBoxData>