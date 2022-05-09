#pragma once
#include "Mediator.h"
#include <fstream>
#include <portability.h>
#include <SDL2/SDL_main.h>
#include "../rppUtils/RppUtils.hpp"
#include "IndicatorConfig.h"

#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING 1
#include <experimental/filesystem>

namespace filesystem = std::experimental::filesystem;

/// @brief Creates an implementation of the mediator
#define CREATE_MEDIATOR_IMPLEMENTATION(type)                                                                                                            \
    template InterventionType Mediator<type>::GetInterventionType();                                                                                    \
    template tIndicator Mediator<type>::GetIndicatorSettings();                                                                                         \
    template tParticipantControl Mediator<type>::GetPControlSettings();                                                                                 \
    template void Mediator<type>::SetTask(Task p_task);                                                                                                 \
    template void Mediator<type>::SetIndicatorSettings(tIndicator p_indicators);                                                                        \
    template void Mediator<type>::SetInterventionType(InterventionType p_type);                                                                         \
    template void Mediator<type>::SetPControlSettings(tParticipantControl p_pControl);                                                                  \
    template void Mediator<type>::SetMaxTime(int p_maxTime);                                                                                            \
    template void Mediator<type>::SetUserId(char* p_userId);                                                                                            \
    template void Mediator<type>::SetDataCollectionSettings(tDataToStore p_dataSetting);                                                                \
    template void Mediator<type>::SetBlackBoxFilePath(const char* p_filePath);                                                                          \
    template void Mediator<type>::DriveTick(tCarElt* p_car, tSituation* p_situation);                                                                   \
    template void Mediator<type>::SetReplayFolder(const filesystem::path& p_replayFolder);                                                              \
    template const filesystem::path& Mediator<type>::GetReplayFolder() const;                                                                           \
    template void Mediator<type>::RaceStart(tTrack* p_track, void* p_carHandle, void** p_carParmHandle, tSituation* p_situation, Recorder* p_recorder); \
    template void Mediator<type>::RaceStop();                                                                                                           \
    template Mediator<type>* Mediator<type>::GetInstance();

/// @brief        Sets the task in SDAConfig to p_task
/// @param p_task The Task
template <typename DecisionMaker>
void Mediator<DecisionMaker>::SetTask(Task p_task)
{
    m_decisionMaker.Config.SetTask(p_task);
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

/// @brief              Does one drive tick in the framework
/// @param  p_car       The current car
/// @param  p_situation The current situation
template <typename DecisionMaker>
void Mediator<DecisionMaker>::DriveTick(tCarElt* p_car, tSituation* p_situation)
{
    CarController.SetCar(p_car);
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
    m_track = p_track;
    tCarElt car;
    bool recordBB = GetPControlSettings().BBRecordSession;

    const char* blackBoxFilePath = m_decisionMaker.Config.GetBlackBoxFilePath();
    std::cout << blackBoxFilePath << std::endl;

    // Load indicators from XML used for assisting the human with visual/audio indicators.
    char path[PATH_BUF_SIZE];
    snprintf(path, PATH_BUF_SIZE, CONFIG_XML_DIR_FORMAT, GfDataDir());
    IndicatorConfig::GetInstance()->LoadIndicatorData(path);

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
    m_decisionMaker.RaceStop();
    m_inRace = false;
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
