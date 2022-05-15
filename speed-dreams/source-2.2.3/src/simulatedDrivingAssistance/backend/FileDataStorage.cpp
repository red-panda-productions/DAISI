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
    m_compressionStep = 0;
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
int FileDataStorage::GetCompressionRate()
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

    // save to the file at the end of the compression time step
    if (m_compressionStep % m_compressionRate == 0)
    {
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
    }
}

void FileDataStorage::SaveCarData(tCarElt* p_car)
{
    Posd pos = p_car->pub.DynGCg.pos;
    tDynPt mov = p_car->pub.DynGC;
    AddForAveraging(m_totalPosX, pos.x);                               // x-position
    AddForAveraging(m_totalPosY, pos.y);                               // y-position
    AddForAveraging(m_totalPosZ, pos.z);                               // z-position
    AddForAveraging(m_totalPosAx, pos.ax);                             // x-direction
    AddForAveraging(m_totalPosAy, pos.ay);                             // y-direction
    AddForAveraging(m_totalPosAz, pos.az);                             // z-direction
    AddForAveraging(m_totalMovVelX, mov.vel.x);                        // speed
    AddForAveraging(m_totalMovAccX, mov.acc.x);                        // acceleration
    AddIntToArray(m_gearValues, p_car->priv.gear, m_compressionStep);  // gear
}

void FileDataStorage::SaveHumanData(tCarElt* p_car)
{
    tCarCtrl ctrl = p_car->ctrl;
    AddToArray(m_steerValues, ctrl.steer, m_compressionStep);       // steer
    AddToArray(m_brakeValues, ctrl.brakeCmd, m_compressionStep);    // brake
    AddToArray(m_accelValues, ctrl.accelCmd, m_compressionStep);    // gas
    AddToArray(m_clutchValues, ctrl.clutchCmd, m_compressionStep);  // clutch
}

void FileDataStorage::SaveInterventionData(DecisionTuple& p_decisions)
{
    if (p_decisions.ContainsSteer())
    {
        AddToArray(m_steerDecision, p_decisions.GetSteer(), m_compressionStep);
    }
    else
    {
        AddToArray(m_steerDecision, -1, m_compressionStep);
    }

    if (p_decisions.ContainsBrake())
    {
        AddToArray(m_brakeDecision, p_decisions.GetBrake(), m_compressionStep);
    }
    else
    {
        AddToArray(m_brakeDecision, -1, m_compressionStep);
    }

    if (p_decisions.ContainsAccel())
    {
        AddToArray(m_accelDecision, p_decisions.GetAccel(), m_compressionStep);
    }
    else
    {
        AddToArray(m_accelDecision, -1, m_compressionStep);
    }

    if (p_decisions.ContainsGear())
    {
        AddIntToArray(m_gearDecision, p_decisions.GetGear(), m_compressionStep);
    }
    else
    {
        AddIntToArray(m_gearDecision, -1, m_compressionStep);
    }

    if (p_decisions.ContainsLights())
    {
        AddIntToArray(m_lightDecision, p_decisions.GetLights(), m_compressionStep);
    }
    else
    {
        AddIntToArray(m_lightDecision, -1, m_compressionStep);
    }
}

void FileDataStorage::WriteCarData()
{
    WRITE_VAR(m_outputStream, GetAverage(m_totalPosX));       // x-position
    WRITE_VAR(m_outputStream, GetAverage(m_totalPosY));       // y-position
    WRITE_VAR(m_outputStream, GetAverage(m_totalPosZ));       // z-position
    WRITE_VAR(m_outputStream, GetAverage(m_totalPosAx));      // x-direction
    WRITE_VAR(m_outputStream, GetAverage(m_totalPosAy));      // y-direction
    WRITE_VAR(m_outputStream, GetAverage(m_totalPosAz));      // z-direction
    WRITE_VAR(m_outputStream, GetAverage(m_totalMovVelX));    // speed
    WRITE_VAR(m_outputStream, GetAverage(m_totalMovAccX));    // acceleration
    WRITE_VAR(m_outputStream, GetLeastCommon(m_gearValues));  // gear
}

void FileDataStorage::WriteHumanData()
{
    WRITE_VAR(m_outputStream, GetMedian(m_steerValues));   // steer
    WRITE_VAR(m_outputStream, GetMedian(m_brakeValues));   // brake
    WRITE_VAR(m_outputStream, GetMedian(m_accelValues));   // gas
    WRITE_VAR(m_outputStream, GetMedian(m_clutchValues));  // clutch
}

void FileDataStorage::WriteInterventionData()
{
    bool decisionMade = false;
    float decision = GetMedian(m_steerDecision);
    if (decision != -1)
    {
        decisionMade = true;
        WRITE_STRING(m_outputStream, "Decisions");
        WRITE_STRING(m_outputStream, "SteerDecision");
        WRITE_VAR(m_outputStream, decision);
    }
    decision = GetMedian(m_brakeDecision);
    if (decision != -1)
    {
        if (!decisionMade)
        {
            decisionMade = true;
            WRITE_STRING(m_outputStream, "Decisions");
        }
        WRITE_STRING(m_outputStream, "BrakeDecision");
        WRITE_VAR(m_outputStream, decision);
    }
    decision = GetMedian(m_accelDecision);
    if (decision != -1)
    {
        if (!decisionMade)
        {
            decisionMade = true;
            WRITE_STRING(m_outputStream, "Decisions");
        }
        WRITE_STRING(m_outputStream, "AccelDecision");
        WRITE_VAR(m_outputStream, decision);
    }
    int intDecision = GetLeastCommon(m_gearDecision);
    if (intDecision != -1)
    {
        if (!decisionMade)
        {
            decisionMade = true;
            WRITE_STRING(m_outputStream, "Decisions");
        }
        WRITE_STRING(m_outputStream, "GearDecision");
        WRITE_VAR(m_outputStream, intDecision);
    }
    intDecision = GetLeastCommon(m_lightDecision);
    if (intDecision != -1)
    {
        if (!decisionMade)
        {
            decisionMade = true;
            WRITE_STRING(m_outputStream, "Decisions");
        }
        WRITE_STRING(m_outputStream, "LightsDecision");
        WRITE_VAR(m_outputStream, intDecision);
    }
    if (!decisionMade)
    {
        WRITE_STRING(m_outputStream, "Decisions");
    }
    WRITE_STRING(m_outputStream, "NONE");
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
float FileDataStorage::GetAverage(float& p_total)
{
    float p_average = p_total / static_cast<float>(m_compressionRate);
    p_total = 0;
    return p_average;
}

/// @brief Add the new value to the array in the correct compression step
/// @param p_values Array with values from the current compression step
/// @param p_value The new value of this timestep for the variable
/// @param p_compressionStep The current compression step
void FileDataStorage::AddToArray(float p_values[], float p_value, unsigned long p_compressionStep)
{
    int p_placeInArray = static_cast<int>(p_compressionStep % static_cast<unsigned long>(m_compressionRate));
    p_values[p_placeInArray] = p_value;
}

/// @brief Add the new integer value to the array in the correct compression step
/// @param p_values Array with values from the current compression step
/// @param p_value The new value of this timestep for the variable
/// @param p_compressionStep The current compression step
void FileDataStorage::AddIntToArray(int p_values[], int p_value, unsigned long p_compressionStep)
{
    int p_placeInArray = static_cast<int>(p_compressionStep % static_cast<unsigned long>(m_compressionRate));
    p_values[p_placeInArray] = p_value;
}

/// @brief Get the median of the current compression step
/// @param p_values Array with values from the current compression step
/// @return The median of the past time steps for a variable
float FileDataStorage::GetMedian(float p_values[])
{
    std::sort(p_values, p_values + m_compressionRate);
    int middle = static_cast<int>(std::floor(static_cast<float>(m_compressionRate) / 2));
    return p_values[middle];
}

/// @brief Get the least common value of the current compression step
/// @param p_values Array with values from the current compression step
/// @return The least common in an array for a variable
int FileDataStorage::GetLeastCommon(int p_values[])
{
    std::map<int, int> p_frequencies;

    // list of all values in p_values
    int* p_valuesList = new int[m_compressionRate];
    int p_valueCount = 0;

    // create a map from values in p_values to the corresponding frequencies
    for (int i = 0; i < m_compressionRate; i++)
    {
        if (p_frequencies[p_values[i]])
        {
            p_frequencies[p_values[i]]++;
        }
        else
        {
            p_frequencies[p_values[i]] = 1;
            p_valuesList[p_valueCount] = p_values[i];
            p_valueCount++;
        }
    }

    int p_leastCommon;
    int p_minCount = m_compressionRate + 1;

    // find the value with the lowest frequency
    for (int i = 0; i < p_valueCount; i++)
    {
        if (p_frequencies[p_valuesList[i]] < p_minCount)
        {
            p_leastCommon = p_valuesList[i];
            p_minCount = p_frequencies[p_valuesList[i]];
        }
    }

    return p_leastCommon;
}