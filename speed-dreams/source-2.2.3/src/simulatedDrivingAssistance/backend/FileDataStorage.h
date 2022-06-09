#pragma once
#include <fstream>
#include "BlackBoxData.h"
#include "ConfigEnums.h"
#include "DecisionTuple.h"
#include <ctime>
#include "Random.hpp"
#include "FileSystem.hpp"

/// @brief the max size of the array in which data is stored
#define COMPRESSION_LIMIT 50

#define TIMESTEPS_CSV_HEADER "tick"
#define GAMESTATE_CSV_HEADER "tick, x, y, z, direction_x, direction_y, direction_z, speed, acceleration, gear"
#define USERINPUT_CSV_HEADER "tick, steer, brake, gas, clutch"
#define DECISIONS_CSV_HEADER "tick, steer_decision, brake_decision, accel_decision, gear_decision, lights_decision"

/// @brief A class that can store simulation data to buffer files
class FileDataStorage
{
public:
    FileDataStorage() = default;
    ~FileDataStorage() = default;

    void Shutdown();

    tBufferPaths Initialize(
        tDataToStore p_saveSettings,
        const std::string& p_userId,
        const std::time_t& p_trialStartTime,
        const std::string& p_blackboxFilename,
        const std::string& p_blackboxName,
        const std::time_t& p_blackboxTime,
        const std::string& p_environmentFilename,
        const std::string& p_environmentName,
        int p_environmentVersion,
        InterventionType p_interventionType);

    void Save(tCarElt* p_car, const DecisionTuple& p_decisions, unsigned long p_timestamp);

    /// @brief Add the new value to the array in the correct compression step
    /// @param p_values          Array with values from the current compression step
    /// @param p_value           The new value of this time step for the variable
    /// @param p_compressionStep The current compression step
    template <typename TNumber>
    void AddToArray(TNumber* p_values, TNumber p_value, int p_compressionStep) const
    {
        p_values[p_compressionStep] = p_value;
    }

    void SetCompressionRate(int p_compressionRate);
    int GetCompressionRate() const;

    void AddForAveraging(float& p_total, float p_value);
    float GetAverage(float& p_total) const;
    float GetMedian(float* p_values);
    int GetLeastCommon(int* p_values) const;

private:
    /// @brief Boolean array to determine what to save and what not to save. Uses indices as in ConfigEnums.h
    tDataToStore m_saveSettings = {};

    std::ofstream m_timeStepsStream = {};
    std::ofstream m_gameStateStream = {};
    std::ofstream m_userInputStream = {};
    std::ofstream m_decisionsStream = {};

    /// @brief Integer that determines the compression rate
    int m_compressionRate = 1;

    /// @brief Integer that determines the current step of the compression
    int m_compressionStep = 0;

    float m_totalPosX = 0, m_totalPosY = 0, m_totalPosZ = 0, m_totalPosAx = 0, m_totalPosAy = 0, m_totalPosAz = 0;
    float m_totalMovVelX = 0, m_totalMovAccX = 0;
    int m_gearValues[COMPRESSION_LIMIT] = {};
    float m_steerValues[COMPRESSION_LIMIT] = {};
    float m_brakeValues[COMPRESSION_LIMIT] = {};
    float m_accelValues[COMPRESSION_LIMIT] = {};
    float m_clutchValues[COMPRESSION_LIMIT] = {};
    float m_steerDecision[COMPRESSION_LIMIT] = {};
    float m_brakeDecision[COMPRESSION_LIMIT] = {};
    float m_accelDecision[COMPRESSION_LIMIT] = {};
    int m_gearDecision[COMPRESSION_LIMIT] = {};
    int m_lightDecision[COMPRESSION_LIMIT] = {};

    Random m_random;

    void SaveCarData(tCarElt* p_car);
    void SaveHumanData(const tCarCtrl& p_ctrl);
    void SaveInterventionData(const DecisionTuple& p_decisions);

    template <typename TNumber>
    void SaveDecision(bool p_decisionMade, TNumber p_value, TNumber* p_values, int p_compressionStep);

    void WriteCarData(unsigned long p_timestamp);
    void WriteHumanData(unsigned long p_timestamp);
    void WriteInterventionData(unsigned long p_timestamp);

    template <typename TNumber>
    void WriteDecision(TNumber p_value, char p_separator);

    void GetMedianUtil(float* p_values, int p_start, int p_end, int p_middle, float& p_startPartition, float& p_endPartition);
    int RandomPartition(float* p_values, int p_start, int p_end);
    int Partition(float* p_values, int p_start, int p_end);
};

/// @brief Standard implementation of the file data storage
#define SFileDataStorage FileDataStorage<BlackBoxData>