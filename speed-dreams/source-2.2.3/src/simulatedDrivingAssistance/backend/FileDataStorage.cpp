#include <ostream>
#include "FileDataStorage.h"
#include <map>
#include "FileSystem.hpp"
#include <iostream>

// Helper functions to write to the buffer file streams
#define WRITE_CSV(stream, val)  stream << val << ','
#define WRITE_LINE(stream, val) stream << val << '\n'

// Some random value that will not occur in a decision
#define SKIP_DECISION -123456

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
    WRITE_LINE(p_stream, buffer);
}

/// @brief Initialize the file data storage, writes meta data to a one of the buffer files.
///        Sets up up all other file streams to the buffer files.
/// @param p_saveSettings        Settings for what data to store.
/// @param p_userId              User ID of the current player.
/// @param p_trialStartTime      Start time of the current race
/// @param p_blackboxFilename    Filename without path (e.g. "blackbox.exe") for the current black box
/// @param p_blackboxName        Name of the current black box (e.g. "My Lane Keeping Algorithm")
/// @param p_blackboxTime        Timestamp for when the current black box was last updated
/// @param p_environmentFilename Filename without path (e.g. "highway.xml") for the current environment
/// @param p_environmentName     Name of the current environment (e.g. "Espie Circuit")
/// @param p_environmentVersion  Version of the current environment
/// @param p_interventionType    Intervention type for the current race
/// @return                      The path to the directory containing the buffer files
tBufferPaths FileDataStorage::Initialize(
    tDataToStore p_saveSettings,
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
    filesystem::path bufferDirectory = filesystem::temp_directory_path().append(BUFFER_TEMP_DIRECTORY);
    filesystem::create_directory(bufferDirectory);

    tBufferPaths bufferPaths = {
        filesystem::path(bufferDirectory).append(BUFFER_FILE_META_DATA),
        filesystem::path(bufferDirectory).append(BUFFER_FILE_TIMESTEPS),
        filesystem::path(bufferDirectory).append(BUFFER_FILE_GAMESTATE),
        filesystem::path(bufferDirectory).append(BUFFER_FILE_USERINPUT),
        filesystem::path(bufferDirectory).append(BUFFER_FILE_DECISIONS),
    };

    std::ofstream metaDataStream(bufferPaths.MetaData);
    m_timeStepsStream.open(bufferPaths.TimeSteps);
    m_gameStateStream.open(bufferPaths.GameState);
    m_userInputStream.open(bufferPaths.UserInput);
    m_decisionsStream.open(bufferPaths.Decisions);

    // Initialize member variables
    m_saveSettings = p_saveSettings;
    m_totalPosX = 0;
    m_totalPosY = 0;
    m_totalPosZ = 0;
    m_totalPosAx = 0;
    m_totalPosAy = 0;
    m_totalPosAz = 0;
    m_totalMovVelX = 0;
    m_totalMovAccX = 0;

    // Write meta-data to file.
    WRITE_LINE(metaDataStream, p_userId);
    WRITE_LINE(metaDataStream, p_blackboxFilename);
    WriteTime(metaDataStream, p_blackboxTime);
    WRITE_LINE(metaDataStream, p_blackboxName);
    WRITE_LINE(metaDataStream, p_environmentFilename);
    WRITE_LINE(metaDataStream, p_environmentVersion);
    WRITE_LINE(metaDataStream, p_environmentName);
    WRITE_LINE(metaDataStream, p_interventionType);
    WriteTime(metaDataStream, p_trialStartTime);

    metaDataStream.close();

    // Write a header to the buffer files to help with future debugging.
    WRITE_LINE(m_timeStepsStream, TIMESTEPS_CSV_HEADER);
    WRITE_LINE(m_gameStateStream, GAMESTATE_CSV_HEADER);
    WRITE_LINE(m_userInputStream, USERINPUT_CSV_HEADER);
    WRITE_LINE(m_decisionsStream, DECISIONS_CSV_HEADER);

    return bufferPaths;
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
/// End result: any possible final data is written and the files are released.
void FileDataStorage::Shutdown()
{
    m_timeStepsStream.close();
    m_gameStateStream.close();
    m_userInputStream.close();
    m_decisionsStream.close();
}

/// @brief Save the current simulation data to the buffers
///        Uses data compression to compress data if needed.
/// @param p_car       Current car status in Speed Dreams
/// @param p_timestamp Current tick
void FileDataStorage::Save(tCarElt* p_car, const DecisionTuple& p_decisions, unsigned long p_timestamp)
{
    // Save all values from this time step into a buffer array for compression.
    if (m_saveSettings.CarData) SaveCarData(p_car);
    if (m_saveSettings.HumanData) SaveHumanData(p_car->ctrl);
    if (m_saveSettings.InterventionData) SaveInterventionData(p_decisions);

    // Advance a compression check and return from function if data should not be saved yet.
    m_compressionStep++;
    if (m_compressionStep != m_compressionRate) return;

    // Otherwise write all data to the files from the compression arrays.
    WRITE_LINE(m_timeStepsStream, p_timestamp);

    if (m_saveSettings.CarData) WriteCarData(p_timestamp);
    if (m_saveSettings.HumanData) WriteHumanData(p_timestamp);
    if (m_saveSettings.InterventionData) WriteInterventionData(p_timestamp);

    // Reset compression
    m_compressionStep = 0;
}

/// @brief Saves the car data from the last time step
/// @param p_car The car object to save
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
/// @param p_ctrl The human control struct
void FileDataStorage::SaveHumanData(const tCarCtrl& p_ctrl)
{
    AddToArray<float>(m_steerValues, p_ctrl.steer, m_compressionStep);
    AddToArray<float>(m_brakeValues, p_ctrl.brakeCmd, m_compressionStep);
    AddToArray<float>(m_accelValues, p_ctrl.accelCmd, m_compressionStep);
    AddToArray<float>(m_clutchValues, p_ctrl.clutchCmd, m_compressionStep);
}

/// @brief Saves the intervention data from the last time step
/// @param p_decisions The decision data to store
void FileDataStorage::SaveInterventionData(const DecisionTuple& p_decisions)
{
    SaveDecision(p_decisions.ContainsSteer(), p_decisions.GetSteer(), m_steerDecision, m_compressionStep);
    SaveDecision(p_decisions.ContainsBrake(), p_decisions.GetBrake(), m_brakeDecision, m_compressionStep);
    SaveDecision(p_decisions.ContainsAccel(), p_decisions.GetAccel(), m_accelDecision, m_compressionStep);
    SaveDecision(p_decisions.ContainsGear(), p_decisions.GetGear(), m_gearDecision, m_compressionStep);
    SaveDecision(p_decisions.ContainsLights(), static_cast<int>(p_decisions.GetLights()), m_lightDecision, m_compressionStep);
}

/// @brief Saves the decision data from the last time step
/// @param p_decisionMade    Boolean that determines whether a decision is made
/// @param p_value           The value of the decision
/// @param p_values          Array of values from previous decisions
/// @param p_compressionStep The current compression step
template <typename TNumber>
void FileDataStorage::SaveDecision(bool p_decisionMade, TNumber p_value, TNumber* p_values, int p_compressionStep)
{
    if (p_decisionMade)
    {
        AddToArray<TNumber>(p_values, p_value, p_compressionStep);
        return;
    }
    AddToArray<TNumber>(p_values, SKIP_DECISION, p_compressionStep);
}

/// @brief Writes the car data from the last m_compressionRate time steps to the buffer file
/// @param p_timestamp The current simulation tick.
void FileDataStorage::WriteCarData(unsigned long p_timestamp)
{
    WRITE_CSV(m_gameStateStream, p_timestamp);
    WRITE_CSV(m_gameStateStream, GetAverage(m_totalPosX));       // x-position
    WRITE_CSV(m_gameStateStream, GetAverage(m_totalPosY));       // y-position
    WRITE_CSV(m_gameStateStream, GetAverage(m_totalPosZ));       // z-position
    WRITE_CSV(m_gameStateStream, GetAverage(m_totalPosAx));      // x-rotation
    WRITE_CSV(m_gameStateStream, GetAverage(m_totalPosAy));      // y-rotation
    WRITE_CSV(m_gameStateStream, GetAverage(m_totalPosAz));      // z-rotation
    WRITE_CSV(m_gameStateStream, GetAverage(m_totalMovVelX));    // speed
    WRITE_CSV(m_gameStateStream, GetAverage(m_totalMovAccX));    // acceleration
    WRITE_LINE(m_gameStateStream, GetLeastCommon(m_gearValues)); // gear
}

/// @brief Writes the human input data from the last m_compressionRate time steps to the buffer file
/// @param p_timestamp The current simulation tick.
void FileDataStorage::WriteHumanData(unsigned long p_timestamp)
{
    WRITE_CSV(m_userInputStream, p_timestamp);
    WRITE_CSV(m_userInputStream, GetMedian(m_steerValues));   // steer
    WRITE_CSV(m_userInputStream, GetMedian(m_brakeValues));   // brake
    WRITE_CSV(m_userInputStream, GetMedian(m_accelValues));   // gas
    WRITE_LINE(m_userInputStream, GetMedian(m_clutchValues)); // clutch
}

/// @brief Writes the intervention data from the last m_compressionRate time steps to the buffer file
/// @param p_timestamp The current simulation tick.
void FileDataStorage::WriteInterventionData(unsigned long p_timestamp)
{
    WRITE_CSV(m_decisionsStream, p_timestamp);
    WriteDecision(GetMedian(m_steerDecision), ',');
    WriteDecision(GetMedian(m_brakeDecision), ',');
    WriteDecision(GetMedian(m_accelDecision), ',');
    WriteDecision(GetLeastCommon(m_gearDecision), ',');
    WriteDecision(GetLeastCommon(m_lightDecision), '\n');
}

/// @brief Writes a decision to the decision buffer file. In case the decision should be skipped,
///        it writes '\N' which corresponds to NULL in the MySQL LOAD DATA INFILE statement
/// @tparam TNumber  The type of the decision value
/// @param p_value   The value corresponding to the decision
/// @param separator The separator to append after the value
template <typename TNumber>
void FileDataStorage::WriteDecision(TNumber p_value, char separator)
{
    if (p_value == static_cast<TNumber>(SKIP_DECISION))
    {
        m_decisionsStream << "\\N";
    }
    else
    {
        m_decisionsStream << p_value;
    }
    m_decisionsStream << separator;
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