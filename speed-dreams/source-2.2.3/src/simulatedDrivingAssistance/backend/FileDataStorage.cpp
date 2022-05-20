#include <ostream>
#include "FileDataStorage.h"
#include <map>

#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING 1

#include <experimental/filesystem>
#include <iostream>

// Write a string to the stream, as the string in text format (without conversion)
#define WRITE_STRING(stream, string) stream << string << "\n"
// Write a variable to the stream
#define WRITE_VAR(stream, val) stream << std::to_string(val) << "\n"  // Binary: stream.write(reinterpret_cast<const char*>(&val), sizeof(val)); stream << "\n"

/// @brief Write a time variable to a stream as a DateTime entry (aka as a "YYYY-MM-DD hh:mm:ss" string),
///  finished by a newline.
/// @param stream Output stream to write time to.
/// @param date Time to format and write to the stream.
inline void WriteTime(std::ostream& p_stream, time_t p_date)
{
    // "YYYY-MM-DD hh:mm:ss" is 19 characters, finishing with a nullpointer makes 20.
    // Thus allocate space for 20 characters.
    char buffer[20];
    strftime(buffer, 20, "%F %T", gmtime(&p_date));
    p_stream << buffer << "\n";
}

/// @brief Initialize the file data storage.
/// End result: a file is created at the given filepath, and initial data is written to the file.
/// @param p_saveSettings Settings for what data to store.
/// @param p_fileName Path of the file to save.
/// @param p_userId User ID of the current player.
/// @param p_trialStartTime Start time of the current race
/// @param p_blackboxFilename Filename without path (e.g. "blackbox.exe") for the current black box
/// @param p_blackboxName Name of the current black box (e.g. "My Lane Keeping Algorithm")
/// @param p_blackboxTime Timestamp for when the current black box was last updated
/// @param p_environmentFilename Filename without path (e.g. "highway.xml") for the current environment
/// @param p_environmentName Name of the current environment (e.g. "Espie Circuit")
/// @param p_environmentVersion Version of the current environment
/// @param p_interventionType Intervention type for the current race
/// @return returns the path of the buffer file
std::experimental::filesystem::path FileDataStorage::Initialize(
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
    InterventionType p_interventionType)
{
    // Create file directory if not yet exists
    std::experimental::filesystem::path filePath = std::experimental::filesystem::temp_directory_path();
    filePath.append(p_fileName.c_str());
    create_directories(filePath.parent_path());

    // Initialize member variables
    m_saveSettings = p_saveSettings;
    m_outputStream.open(filePath);
    m_totalPosX = 0;
    m_totalPosY = 0;
    m_totalPosZ = 0;
    m_totalPosAx = 0;
    m_totalPosAy = 0;
    m_totalPosAz = 0;
    m_totalMovVelX = 0;
    m_totalMovAccX = 0;

    // User and trial data
    WRITE_STRING(m_outputStream, p_userId);
    WriteTime(m_outputStream, p_trialStartTime);
    // Black box data
    WRITE_STRING(m_outputStream, p_blackboxFilename);
    WriteTime(m_outputStream, p_blackboxTime);
    WRITE_STRING(m_outputStream, p_blackboxName);
    // Environment data
    WRITE_STRING(m_outputStream, p_environmentFilename);
    WRITE_VAR(m_outputStream, p_environmentVersion);
    WRITE_STRING(m_outputStream, p_environmentName);
    // Intervention data
    WRITE_VAR(m_outputStream, p_interventionType);
    // Headers to indicate what data will be saved
    if (m_saveSettings.EnvironmentData)
    {
        WRITE_STRING(m_outputStream, "GameState");
    }
    if (m_saveSettings.HumanData)
    {
        WRITE_STRING(m_outputStream, "UserInput");
    }
    m_outputStream.flush();

    return {filePath};
}

/// @brief Sets the compression rate of the file data storage
void FileDataStorage::SetCompressionRate(int p_compressionRate)
{
    m_compressionRate = p_compressionRate;
}

/// @brief Gets the compression rate of the file data storage
int FileDataStorage::GetCompressionRate() const
{
    return m_compressionRate;
}

/// @brief Shutdown the file data storage.
/// End result: any possible final data is written and the file is released.
void FileDataStorage::Shutdown()
{
    m_outputStream << "END";
    m_outputStream.close();
}

/// @brief Save the current driving situation to the buffer
/// @param p_car Current car status in Speed Dreams
/// @param p_situation Current situation in Speed Dreams
/// @param p_timestamp Current tick
void FileDataStorage::Save(tCarElt* p_car, tSituation* p_situation, DecisionTuple& p_decisions, unsigned long p_timestamp)
{
    // save all values from this time step
    if (m_saveSettings.CarData)
    {
        SaveCarData(p_car);
    }
    if (m_saveSettings.HumanData)
    {
        SaveHumanData(p_car);
    }

    if (m_saveSettings.InterventionData)
    {
        SaveInterventionData(p_decisions);
    }

    m_compressionStep++;

    if (m_compressionStep != m_compressionRate) return;

    // save to the file at the end of the compression time step
    WRITE_VAR(m_outputStream, p_timestamp);
    if (m_saveSettings.CarData)
    {
        WriteCarData();
    }
    if (m_saveSettings.HumanData)
    {
        WriteHumanData();
    }
    if (m_saveSettings.InterventionData)
    {
        WriteInterventionData();
    }
    m_compressionStep = 0;
}

/// @brief Saves the car data from the last time step
void FileDataStorage::SaveCarData(tCarElt* p_car)
{
    Posd pos = p_car->pub.DynGCg.pos;
    tDynPt mov = p_car->pub.DynGC;
    AddForAveraging(m_totalPosX, pos.x);                                 // x-position
    AddForAveraging(m_totalPosY, pos.y);                                 // y-position
    AddForAveraging(m_totalPosZ, pos.z);                                 // z-position
    AddForAveraging(m_totalPosAx, pos.ax);                               // x-direction
    AddForAveraging(m_totalPosAy, pos.ay);                               // y-direction
    AddForAveraging(m_totalPosAz, pos.az);                               // z-direction
    AddForAveraging(m_totalMovVelX, mov.vel.x);                          // speed
    AddForAveraging(m_totalMovAccX, mov.acc.x);                          // acceleration
    AddToArray<int>(m_gearValues, p_car->priv.gear, m_compressionStep);  // gear
}

/// @brief Saves the human data from the last time step
void FileDataStorage::SaveHumanData(tCarElt* p_car)
{
    tCarCtrl ctrl = p_car->ctrl;
    AddToArray<float>(m_steerValues, ctrl.steer, m_compressionStep);
    AddToArray<float>(m_brakeValues, ctrl.brakeCmd, m_compressionStep);
    AddToArray<float>(m_accelValues, ctrl.accelCmd, m_compressionStep);
    AddToArray<float>(m_clutchValues, ctrl.clutchCmd, m_compressionStep);
}

/// @brief Saves the intervention data from the last time step
void FileDataStorage::SaveInterventionData(DecisionTuple& p_decisions)
{
    SaveDecision(p_decisions.ContainsSteer(), p_decisions.GetSteer(), m_steerDecision, m_compressionStep);
    SaveDecision(p_decisions.ContainsBrake(), p_decisions.GetBrake(), m_brakeDecision, m_compressionStep);
    SaveDecision(p_decisions.ContainsAccel(), p_decisions.GetAccel(), m_accelDecision, m_compressionStep);
    SaveDecision(p_decisions.ContainsGear(), p_decisions.GetGear(), m_gearDecision, m_compressionStep);
    SaveDecision(p_decisions.ContainsLights(), static_cast<int>(p_decisions.GetLights()), m_lightDecision, m_compressionStep);
}

/// @brief Saves the decision data from the last time step
/// @param p_decisionMade boolean that determines whether a decision is made
/// @param p_value the value of the decision
/// @param p_values array of values from previous decisions
/// @param p_compressionStep the current step the program is in
template <typename TNumber>
void FileDataStorage::SaveDecision(bool p_decisionMade, TNumber p_value, TNumber* p_values, int p_compressionStep)
{
    if (p_decisionMade)
    {
        AddToArray<TNumber>(p_values, p_value, p_compressionStep);
        return;
    }
    AddToArray<TNumber>(p_values, -1, p_compressionStep);
}

/// @brief Writes the car data from the last m_compressionRate time steps to the buffer file
void FileDataStorage::WriteCarData()
{
    WRITE_VAR(m_outputStream, GetAverage(m_totalPosX));       // x-position
    WRITE_VAR(m_outputStream, GetAverage(m_totalPosY));       // y-position
    WRITE_VAR(m_outputStream, GetAverage(m_totalPosZ));       // z-position
    WRITE_VAR(m_outputStream, GetAverage(m_totalPosAx));      // x-rotation
    WRITE_VAR(m_outputStream, GetAverage(m_totalPosAy));      // y-rotation
    WRITE_VAR(m_outputStream, GetAverage(m_totalPosAz));      // z-rotation
    WRITE_VAR(m_outputStream, GetAverage(m_totalMovVelX));    // speed
    WRITE_VAR(m_outputStream, GetAverage(m_totalMovAccX));    // acceleration
    WRITE_VAR(m_outputStream, GetLeastCommon(m_gearValues));  // gear
}

/// @brief Writes the human data from the last m_compressionRate time steps to the buffer file
void FileDataStorage::WriteHumanData()
{
    WRITE_VAR(m_outputStream, GetMedian(m_steerValues));   // steer
    WRITE_VAR(m_outputStream, GetMedian(m_brakeValues));   // brake
    WRITE_VAR(m_outputStream, GetMedian(m_accelValues));   // gas
    WRITE_VAR(m_outputStream, GetMedian(m_clutchValues));  // clutch
}

/// @brief Writes the intervention data from the last m_compressionRate time steps to the buffer file
void FileDataStorage::WriteInterventionData()
{
    bool decisionMade = false;
    WriteDecision(GetMedian(m_steerDecision), "SteerDecision", decisionMade);
    WriteDecision(GetMedian(m_brakeDecision), "BrakeDecision", decisionMade);
    WriteDecision(GetMedian(m_accelDecision), "AccelDecision", decisionMade);
    WriteDecision(GetLeastCommon(m_gearDecision), "GearDecision", decisionMade);
    WriteDecision(GetLeastCommon(m_lightDecision), "LightsDecision", decisionMade);

    if (!decisionMade)
    {
        WRITE_STRING(m_outputStream, "Decisions");
    }
    WRITE_STRING(m_outputStream, "NONE");
}

/// @brief Writes the p_decisionType data from the last m_compressionRate time steps to the buffer file
/// @param p_decision the value of the p_decisionType of the last time m_comrpessionRate time steps
/// @param p_decisionType the current decision type
/// @param p_decisionMade if there has already been a decision made
template <typename TNumber>
void FileDataStorage::WriteDecision(TNumber p_decision, const std::string& p_decisionType, bool& p_decisionMade)
{
    if (p_decision == -1) return;

    if (!p_decisionMade)
    {
        p_decisionMade = true;
        WRITE_STRING(m_outputStream, "Decisions");
    }

    WRITE_STRING(m_outputStream, p_decisionType);
    WRITE_VAR(m_outputStream, p_decision);
}

/// @brief Add the new value to the total of the current time steps
/// @param p_total The current total of the variable
/// @param p_value The new value of this timestep for the variable
void FileDataStorage::AddForAveraging(float& p_total, float p_value)
{
    p_total += p_value;
}

/// @brief Calculates the average of the past time steps for a variable and resets it
/// @param p_total The current total of the variable
/// @return The average of the past time steps for a variable
float FileDataStorage::GetAverage(float& p_total) const
{
    float average = p_total / static_cast<float>(m_compressionRate);
    p_total = 0;
    return average;
}

/// @brief Add the new value to the array in the correct compression step
/// @param p_values Array with values from the current compression step
/// @param p_value The new value of this time step for the variable
/// @param p_compressionStep The current compression step
template <typename TNumber>
void FileDataStorage::AddToArray(TNumber p_values[], TNumber p_value, int p_compressionStep) const
{
    p_values[p_compressionStep] = p_value;
}

/// @brief Get the median of the current compression step using Randomized QuickSelect
/// @param p_values Array with values from the current compression step
/// @return The median of the past time steps for a variable
float FileDataStorage::GetMedian(float* p_values)
{
    float startPartition = -1, median = -1;
    GetMedianUtil(p_values, 0, m_compressionRate - 1, static_cast<int>(m_compressionRate / 2), startPartition, median);
    return median;
}

/// @brief Utility function to recursively get the median
/// @param p_values Array with values from the current compression step
/// @param p_start start of the current section of the array
/// @param p_end end of the current section of the array
/// @param p_middle middle of the current section of the array
/// @param p_a number before the median of p_values
/// @param p_b median of p_values
void FileDataStorage::GetMedianUtil(float* p_values, int p_start, int p_end, int p_middle, float& p_startPartition, float& p_endPartition)
{
    if (p_start > p_end) return;

    int partitionIndex = RandomPartition(p_values, p_start, p_end);

    if (partitionIndex == p_middle)
    {
        p_endPartition = p_values[partitionIndex];
        if (p_startPartition != -1) return;
    }
    else if (partitionIndex == p_middle - 1)
    {
        p_startPartition = p_values[partitionIndex];
        if (p_endPartition != -1) return;
    }

    if (partitionIndex >= p_middle)
    {
        GetMedianUtil(p_values, p_start, partitionIndex - 1, p_middle, p_startPartition, p_endPartition);
        return;
    }
    GetMedianUtil(p_values, partitionIndex + 1, p_end, p_middle, p_startPartition, p_endPartition);
}

/// @brief partitions p_values around a randomly picked element
/// @param p_values Array with values from the current compression step
/// @param p_start start of the partition
/// @param p_end end of the partition
int FileDataStorage::RandomPartition(float* p_values, int p_start, int p_end)
{
    int pivot = m_random.NextInt(p_start, p_end + 1);
    std::swap(p_values[pivot], p_values[p_end]);
    return Partition(p_values, p_start, p_end);
}

/// @brief returns the correct position of the pivot element
/// @param p_values Array with values from the current compression step
/// @param p_start start of the partition
/// @param p_end end of the partition
int FileDataStorage::Partition(float* p_values, int p_start, int p_end)
{
    float lastValue = p_values[p_end];
    int i = p_start, j = p_start;
    while (j < p_end)
    {
        if (p_values[j] < lastValue)
        {
            std::swap(p_values[i], p_values[j]);
            i++;
        }
        j++;
    }
    std::swap(p_values[i], p_values[j]);
    return i;
}

/// @brief Get the least common value of the current compression step
/// @param p_values Array with values from the current compression step
/// @return The least common in an array for a variable
int FileDataStorage::GetLeastCommon(int* p_values) const
{
    std::map<int, int> frequencies;

    // list of all values in p_values
    int valuesList[COMPRESSION_LIMIT];
    int valueCount = 0;

    // create a map from values in p_values to the corresponding frequencies
    for (int i = 0; i < m_compressionRate; i++)
    {
        if (frequencies[p_values[i]])
        {
            frequencies[p_values[i]]++;
            continue;
        }
        frequencies[p_values[i]] = 1;
        valuesList[valueCount] = p_values[i];
        valueCount++;
    }

    int leastCommon;
    int minCount = m_compressionRate + 1;

    // find the value with the lowest frequency
    for (int i = 0; i < valueCount; i++)
    {
        if (frequencies[valuesList[i]] >= minCount) continue;
        leastCommon = valuesList[i];
        minCount = frequencies[valuesList[i]];
    }

    return leastCommon;
}