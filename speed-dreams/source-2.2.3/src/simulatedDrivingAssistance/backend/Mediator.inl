#pragma once
#include "Mediator.h"
#include <fstream>
#include <portability.h>
#include <SDL2/SDL_main.h>
#include "../rppUtils/RppUtils.hpp"
#include "IndicatorConfig.h"
#include "SQLDatabaseStorage.h"
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING 1
#include <experimental/filesystem>

namespace filesystem = std::experimental::filesystem;

/// @brief Creates an implementation of the mediator
#define CREATE_MEDIATOR_IMPLEMENTATION(type)                                                                                                            \
    template InterventionType Mediator<type>::GetInterventionType();                                                                                    \
    template tAllowedActions Mediator<type>::GetAllowedActions();                                                                                       \
    template tIndicator Mediator<type>::GetIndicatorSettings();                                                                                         \
    template tParticipantControl Mediator<type>::GetPControlSettings();                                                                                 \
    template tDecisionThresholds Mediator<type>::GetThresholdSettings();                                                                                \
    template bool Mediator<type>::GetBlackBoxSyncOption();                                                                                              \
    template bool Mediator<type>::GetReplayRecorderSetting();                                                                                           \
    template int Mediator<type>::GetMaxTime();                                                                                                          \
    template void Mediator<type>::SetAllowedActions(tAllowedActions p_allowedActions);                                                                  \
    template void Mediator<type>::SetIndicatorSettings(tIndicator p_indicators);                                                                        \
    template void Mediator<type>::SetInterventionType(InterventionType p_type);                                                                         \
    template void Mediator<type>::SetPControlSettings(tParticipantControl p_pControl);                                                                  \
    template void Mediator<type>::SetReplayRecorderSetting(bool p_replayRecorderOn);                                                                    \
    template void Mediator<type>::SetMaxTime(int p_maxTime);                                                                                            \
    template void Mediator<type>::SetCompressionRate(int p_compressionRate);                                                                            \
    template void Mediator<type>::SetUserId(char* p_userId);                                                                                            \
    template void Mediator<type>::SetDataCollectionSettings(tDataToStore p_dataSetting);                                                                \
    template void Mediator<type>::SetBlackBoxFilePath(const char* p_filePath);                                                                          \
    template void Mediator<type>::SetEnvironmentFilePath(const char* p_filePath);                                                                       \
    template const char* Mediator<type>::GetEnvironmentFilePath();                                                                                      \
    template void Mediator<type>::SetBlackBoxSyncOption(bool p_sync);                                                                                   \
    template void Mediator<type>::SetThresholdSettings(tDecisionThresholds p_thresholds);                                                               \
    template void Mediator<type>::DriveTick(tCarElt* p_car, tSituation* p_situation);                                                                   \
    template void Mediator<type>::SetReplayFolder(const filesystem::path& p_replayFolder);                                                              \
    template const filesystem::path& Mediator<type>::GetReplayFolder() const;                                                                           \
    template void Mediator<type>::RaceStart(tTrack* p_track, void* p_carHandle, void** p_carParmHandle, tSituation* p_situation, Recorder* p_recorder); \
    template void Mediator<type>::SetSaveRaceToDatabase(bool p_saveToDatabase);                                                                         \
    template void Mediator<type>::RaceStop();                                                                                                           \
    template void Mediator<type>::SetDatabaseSettings(tDatabaseSettings p_dbSettings);                                                                  \
    template DatabaseSettings Mediator<type>::GetDatabaseSettings();                                                                                    \
    template bool Mediator<type>::CheckConnection(DatabaseSettings p_dbSettings);                                                                       \
    template bool Mediator<type>::TimeOut();                                                                                                            \
    template Mediator<type>* Mediator<type>::GetInstance();

/// @brief        Sets the allowed actions in SDAConfig to p_allowedActions
/// @param p_allowedActions The allowed actions
template <typename DecisionMaker>
void Mediator<DecisionMaker>::SetAllowedActions(tAllowedActions p_allowedActions)
{
    m_decisionMaker.Config.SetAllowedActions(p_allowedActions);
}

/// @brief              Sets the settings for indication of interventions
/// @param p_indicators The Indicator settings
template <typename DecisionMaker>
void Mediator<DecisionMaker>::SetIndicatorSettings(tIndicator p_indicators)
{
    m_decisionMaker.Config.SetIndicatorSettings(p_indicators);
}

/// @brief  Gets the intervention type from the decision maker
/// @return The intervention type from the decision maker
template <typename DecisionMaker>
InterventionType Mediator<DecisionMaker>::GetInterventionType()
{
    return m_decisionMaker.Config.GetInterventionType();
}

/// @brief        Sets the settings for interventionType to p_type
/// @param p_type The InterventionType
template <typename DecisionMaker>
void Mediator<DecisionMaker>::SetInterventionType(InterventionType p_type)
{
    m_decisionMaker.ChangeSettings(p_type);
}

/// @brief            Sets the participant control settings to p_pControl
/// @param p_pControl The participant control settings
template <typename DecisionMaker>
void Mediator<DecisionMaker>::SetPControlSettings(tParticipantControl p_pControl)
{
    return m_decisionMaker.Config.SetPControlSettings(p_pControl);
}

/// @brief                    Sets the replay recorder setting to p_replayRecorderOn
/// @param p_replayRecorderOn Whether the replay recorder should be on
template <typename DecisionMaker>
void Mediator<DecisionMaker>::SetReplayRecorderSetting(bool p_replayRecorderOn)
{
    m_decisionMaker.Config.SetReplayRecorderSetting(p_replayRecorderOn);
}

/// @brief           Sets the maximum simulation time to p_maxTime
/// @param p_maxTime The maximum simulation time
template <typename DecisionMaker>
void Mediator<DecisionMaker>::SetMaxTime(int p_maxTime)
{
    m_decisionMaker.Config.SetMaxTime(p_maxTime);
}

/// @brief          Sets the userID to p_userID
/// @param p_userID The userID
template <typename DecisionMaker>
void Mediator<DecisionMaker>::SetUserId(char* p_userId)
{
    m_decisionMaker.Config.SetUserId(p_userId);
}

/// @brief          Sets the compressionLevel to p_compressionLevel
/// @param p_compressionLevel The compression level
template <typename DecisionMaker>
void Mediator<DecisionMaker>::SetCompressionRate(int p_compressionRate)
{
    m_decisionMaker.Config.SetCompressionRate(p_compressionRate);
}

/// @brief               Sets the settings for data collection
/// @param p_dataSetting An array of booleans to enable/disable the collection of simulation data for research
template <typename DecisionMaker>
void Mediator<DecisionMaker>::SetDataCollectionSettings(tDataToStore p_dataSetting)
{
    m_decisionMaker.SetDataCollectionSettings(p_dataSetting);
}

/// @brief            Sets the filepath for the black box executable
/// @param p_filePath A const char* representing the filepath of the black box executable
template <typename DecisionMaker>
void Mediator<DecisionMaker>::SetBlackBoxFilePath(const char* p_filePath)
{
    m_decisionMaker.Config.SetBlackBoxFilePath(p_filePath);
}

/// @brief            Sets the filepath for the environment descriptor xml
/// @param p_filePath A const char* representing the filepath of the environment descriptor xml
template <typename DecisionMaker>
void Mediator<DecisionMaker>::SetEnvironmentFilePath(const char* p_filePath)
{
    m_decisionMaker.Config.SetEnvironmentFilePath(p_filePath);
}

/// @brief Gets the filepath for the environment descriptor xml
/// @return A const char* representing the filepath of the environment descriptor xml
template <typename DecisionMaker>
const char* Mediator<DecisionMaker>::GetEnvironmentFilePath()
{
    return m_decisionMaker.Config.GetEnvironmentFilePath();
}

/// @brief        Sets the sync option of the black box
/// @param p_sync Whether the black box should be run asynchronously (true), or synchronously (false)
template <typename DecisionMaker>
void Mediator<DecisionMaker>::SetBlackBoxSyncOption(bool p_sync)
{
    m_decisionMaker.Config.SetBlackBoxSyncOption(p_sync);
}

/// @brief              Sets the decision threshold values
/// @param p_thresholds The threshold values
template <typename DecisionMaker>
void Mediator<DecisionMaker>::SetThresholdSettings(tDecisionThresholds p_thresholds)
{
    m_thresholds = p_thresholds;
}

/// @brief  Gets the allowed black box actions setting
/// @return The allowed black box actions
template <typename DecisionMaker>
tAllowedActions Mediator<DecisionMaker>::GetAllowedActions()
{
    return m_decisionMaker.Config.GetAllowedActions();
}

/// @brief             Gets the setting for the given indicator
/// @param p_indicator Indicator whose setting to get
/// @return true if the indicator is enabled, false when disabled
template <typename DecisionMaker>
tIndicator Mediator<DecisionMaker>::GetIndicatorSettings()
{
    return m_decisionMaker.Config.GetIndicatorSettings();
}

/// @brief  Gets the participant control settings
/// @return The participant control settings
template <typename DecisionMaker>
tParticipantControl Mediator<DecisionMaker>::GetPControlSettings()
{
    return m_decisionMaker.Config.GetPControlSettings();
}

/// @brief  Returns the decision threshold values
/// @return The threshold values
template <typename DecisionMaker>
tDecisionThresholds Mediator<DecisionMaker>::GetThresholdSettings()
{
    return m_thresholds;
}

/// @brief        Sets the folder that contains all replay data.
/// @param p_replayFolder The path to the folder
template <typename DecisionMaker>
void Mediator<DecisionMaker>::SetReplayFolder(const filesystem::path& p_replayFolder)
{
    m_decisionMaker.Config.SetReplayFolder(p_replayFolder);
}

/// @brief Gets the replay folder if it has been set from the command line.
/// @return The path to the replay folder
template <typename DecisionMaker>
const filesystem::path& Mediator<DecisionMaker>::GetReplayFolder() const
{
    return m_decisionMaker.Config.GetReplayFolder();
}

/// @brief Gets the replay recorder setting
/// @return The replay recorder setting
template <typename DecisionMaker>
bool Mediator<DecisionMaker>::GetReplayRecorderSetting()
{
    return m_decisionMaker.Config.GetReplayRecorderSetting();
}

/// @brief  Gets the black box sync option
/// @return The black box sync option
template <typename DecisionMaker>
bool Mediator<DecisionMaker>::GetBlackBoxSyncOption()
{
    return m_decisionMaker.Config.GetBlackBoxSyncOption();
}

/// @brief  Gets the maximum simulation time
/// @return The maximum simulation time
template <typename DecisionMaker>
int Mediator<DecisionMaker>::GetMaxTime()
{
    return m_decisionMaker.Config.GetMaxTime();
}

/// @brief              Does one drive tick in the framework
/// @param  p_car       The current car
/// @param  p_situation The current situation
template <typename DecisionMaker>
void Mediator<DecisionMaker>::DriveTick(tCarElt* p_car, tSituation* p_situation)
{
    CarControl.SetCar(p_car);
    m_decisionMaker.Decide(p_car, p_situation, m_tickCount);
    m_tickCount++;
}

/// @brief                  Starts the race in the framework
/// @param  p_track         The track of the current race
/// @param  p_carHandle     A car handle (from speed dreams)
/// @param  p_carParmHandle A car parameter handle (from speed dreams)
/// @param  p_situation     The current situation
/// @param  p_recorder      The recorder to use if recording is enabled
template <typename DecisionMaker>
void Mediator<DecisionMaker>::RaceStart(tTrack* p_track, void* p_carHandle, void** p_carParmHandle, tSituation* p_situation, Recorder* p_recorder)
{
    m_tickCount = 0;
    m_track = p_track;
    tCarElt car;
    bool recordBB = GetReplayRecorderSetting();

    const char* blackBoxFilePath = m_decisionMaker.Config.GetBlackBoxFilePath();
    std::cout << blackBoxFilePath << std::endl;

    // Load indicators from XML used for assisting the human with visual/audio indicators.
    char path[PATH_BUF_SIZE];
    snprintf(path, PATH_BUF_SIZE, CONFIG_XML_DIR_FORMAT, GfDataDir());
    IndicatorConfig::GetInstance()->LoadIndicatorData(path, GetInterventionType());

    // Initialize the decision maker with the full path to the current black box executable
    // If recording is disabled a nullptr is passed
    m_decisionMaker.Initialize(m_tickCount, &car, p_situation, p_track, blackBoxFilePath, recordBB ? p_recorder : nullptr);

    m_inRace = true;
}

/// @brief Tells the decisionmaker that the race has ended
template <typename DecisionMaker>
void Mediator<DecisionMaker>::RaceStop()
{
    if (!m_inRace) return;
    bool saveToDatabase = m_decisionMaker.Config.GetSaveToDatabaseCheck();
    m_decisionMaker.RaceStop(saveToDatabase);
    m_inRace = false;
}

/// @brief            Sets the database connection settings for the database server
/// @param p_dbSettings The settings made in the DatabaseSettingsMenu
template <typename DecisionMaker>
void Mediator<DecisionMaker>::SetDatabaseSettings(tDatabaseSettings p_dbSettings)
{
    m_dbSettings = p_dbSettings;
}

/// @brief  Gets the database connection settings
/// @return The database connection settings
template <typename DecisionMaker>
tDatabaseSettings Mediator<DecisionMaker>::GetDatabaseSettings()
{
    return m_dbSettings;
}

/// @brief  Gets the database connection settings
/// @return The database connection settings
template <typename DecisionMaker>
bool Mediator<DecisionMaker>::CheckConnection(DatabaseSettings p_dbSettings)
{
    SQLDatabaseStorage test;
    bool connectable = test.OpenDatabase(p_dbSettings);
    return connectable;
}

/// @brief                  Tells the decionmaker that the experiment data should be saved or not.
/// @param p_saveToDatabase boolean that determines the value of the m_decisionMaker.
template <typename DecisionMaker>
void Mediator<DecisionMaker>::SetSaveRaceToDatabase(bool p_saveToDatabase)
{
    m_decisionMaker.Config.SetSaveToDatabaseCheck(p_saveToDatabase);
}

/// @brief Creates a mediator instance if needed and returns it
/// @return A mediator instance
template <typename DecisionMaker>
Mediator<DecisionMaker>* Mediator<DecisionMaker>::GetInstance()
{
    // If the instance exists, return it.
    // Otherwise create the instance and store it for future calls.
    if (m_instance) return m_instance;

    // Check if Mediator file exists
    struct stat info = {};

    std::experimental::filesystem::path path = SingletonsFilePath();
    path.append("Mediator");
    std::string pathstring = path.string();
    const char* filepath = pathstring.c_str();
    int err = stat(filepath, &info);
    if (err == -1)
    {
        // File does not exist -> create pointer
        m_instance = new Mediator();
        std::ofstream file(filepath);
        file << m_instance;
        file.close();
        return m_instance;
    }

    // File exists -> read pointer
    std::string pointerName("00000000");
    std::ifstream file(filepath);
    getline(file, pointerName);
    file.close();
    int pointerValue = stoi(pointerName, nullptr, 16);
    m_instance = (Mediator<DecisionMaker>*)pointerValue;
    return m_instance;
}

/// @brief returns whether the race has taken longer than the requested amount of minutes
template <typename DecisionMaker>
bool Mediator<DecisionMaker>::TimeOut()
{
    float maxTime = m_decisionMaker.Config.GetMaxTime() * 60;
    float currentTime = m_tickCount * static_cast<float>(RCM_MAX_DT_ROBOTS);
    return maxTime < currentTime;
}