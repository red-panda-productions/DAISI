#include <fstream>
#include <string>
#include <portability.h>
#include <stdexcept>
#include <tgf.h>

#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING 1
#include <experimental/filesystem>

#include "../rppUtils/RppUtils.hpp"

#include "IndicatorConfig.h"
#include "Mediator.h"

/// @brief        Loads the indicator data of every intervention action from config file in the given path
/// @param p_path The path to the XML file containing the indicator data to load
void IndicatorConfig::LoadIndicatorData(const char* p_path)
{
    void* xmlHandle = GfParmReadFile(p_path, GFPARM_RMODE_STD);

    InterventionType interventionType = SMediator::GetInstance()->GetInterventionType();

    // Load the indicator data for every intervention action
    char path[PATH_BUF_SIZE];
    for (int i = 0; i < NUM_INTERVENTION_ACTION; i++)
    {
        snprintf(path, PATH_BUF_SIZE, "%s/%s/", PRM_SECT_INTERVENTIONS, s_actionEnumString[i]);
        m_indicatorData[i] = {
            (InterventionAction)i,
            LoadSound(xmlHandle, std::string(path)),
            LoadTexture(xmlHandle, std::string(path), interventionType),
            LoadText(xmlHandle, std::string(path))};
    }
}

/// @brief  Returns a vector of the indicator data
/// @return The vector of indicator data
std::vector<tIndicatorData> IndicatorConfig::GetIndicatorData()
{
    return m_indicatorData;
}

/// @brief                    Returns a vector containing indicator data for all active indicators,
///                           if and only if the interventions are turned on (not on NO_SIGNALS).
/// @param p_interventionType The intervention type setting
/// @return                   The vector of indicator data
std::vector<tIndicatorData> IndicatorConfig::GetActiveIndicators(InterventionType p_interventionType)
{
    return m_activeIndicators;
}

/// @brief                    Returns a vector containing indicator data for all neutral indicators,
///                           if and only if the interventions are turned on (not on NO_SIGNALS).
/// @param p_interventionType The intervention type setting
/// @return                   The vector of indicator data
std::vector<tIndicatorData> IndicatorConfig::GetNeutralIndicators(InterventionType p_interventionType)
{
    // Guard when no signals are to be sent, always return an empty vector.
    if (p_interventionType == INTERVENTION_TYPE_NO_SIGNALS) return {};

    std::vector<tIndicatorData> activeIndicators = GetActiveIndicators(p_interventionType);
    
    // Guard when there are 2 active indicators, we return an empty vector because there can be no neutral indicators
    if (activeIndicators.size() == 2) return {};

    // For the indicators that are active, get for the indicators that are neutral
    for (const tIndicatorData& indicator : GetActiveIndicators(p_interventionType))
    {
        // This if-statement is build around the idea that either braking or steering is currently active, not both
        if (indicator.Action == INTERVENTION_ACTION_BRAKE_NONE || indicator.Action == INTERVENTION_ACTION_BRAKE || indicator.Action == INTERVENTION_ACTION_ACCELERATE)
        {
            // Only steering is in neutral
            tIndicatorData m_neutralSteer = IndicatorConfig::GetInstance()->GetNeutralIndicator(INTERVENTION_ACTION_STEER_NONE);
            m_neutralIndicators = {m_neutralSteer};
        }
        else if (indicator.Action == INTERVENTION_ACTION_STEER_NONE || indicator.Action == INTERVENTION_ACTION_TURN_LEFT || indicator.Action == INTERVENTION_ACTION_TURN_RIGHT)
        {
            // Only braking is in neutral
            tIndicatorData m_neutralBrake = IndicatorConfig::GetInstance()->GetNeutralIndicator(INTERVENTION_ACTION_BRAKE_NONE);
            m_neutralIndicators = {m_neutralBrake};
        }
        else
        {
            // Both braking and steering are in neutral
            tIndicatorData m_neutralSteer = IndicatorConfig::GetInstance()->GetNeutralIndicator(INTERVENTION_ACTION_STEER_NONE);
            tIndicatorData m_neutralBrake = IndicatorConfig::GetInstance()->GetNeutralIndicator(INTERVENTION_ACTION_BRAKE_NONE);
            m_neutralIndicators = {m_neutralSteer, m_neutralBrake};
        }
    }
    return m_neutralIndicators;
}

/// @brief          Returns an element containing indicator data for only a neutral indicator
/// @return         The element of neutral indicator data
tIndicatorData IndicatorConfig::GetNeutralIndicator(InterventionAction action)
{
    std::vector<tIndicatorData> m_neutralIndicator;
    for (const tIndicatorData& indicator : IndicatorConfig::GetInstance()->GetIndicatorData())
    {
        if (indicator.Action == action)
            m_neutralIndicator.insert(m_neutralIndicator.end(), indicator);
    }
    tIndicatorData first = m_neutralIndicator.front();
    return first;
}

/// @brief          Activates the given intervention indicator
/// @param p_action The intervention to activate the indicators for
void IndicatorConfig::ActivateIndicator(InterventionAction p_action)
{
    // TODO: add to the vector instead of overwriting it, this also requires
    //       a way to remove the indicator after some time has passed.
    std::vector<tIndicatorData> m_indicatorData = IndicatorConfig::GetInstance()->GetIndicatorData();
    m_activeIndicators = {m_indicatorData[p_action]};
}

/// @brief          Loads the sound indicator data from the indicator config.xml
/// @param p_handle The p_handle to the config.xml file
/// @param p_path   The p_path to the current intervention action to load
/// @return         The pointer to the struct containing the sound data
tSoundData* IndicatorConfig::LoadSound(void* p_handle, std::string p_path)
{
    p_path += PRM_SECT_SOUND;
    if (!GfParmExistsSection(p_handle, p_path.c_str())) return nullptr;

    tSoundData* data = new SoundData;

    const char* source = GfParmGetStr(p_handle, p_path.c_str(), PRM_ATTR_SRC, "");

    char* sndPath = new char[PATH_BUF_SIZE];
    snprintf(sndPath, PATH_BUF_SIZE, SOUNDS_DIR_FORMAT, GfDataDir(), source);
    data->Path = sndPath;

    data->Looping = strcmp(GfParmGetStr(p_handle, p_path.c_str(), PRM_ATTR_LOOPING, VAL_NO), VAL_YES) == 0;
    data->LoopInterval = GfParmGetNum(p_handle, p_path.c_str(), PRM_ATTR_LOOP_INTERVAL, nullptr, 0);

    // A loop interval of 0 means "loop as fast as possible without sounds overlapping"
    // The actual loop interval is max(sound_length, loop-interval) to guarantee sounds don't overlap
    if (data->LoopInterval < 0)
    {
        throw std::runtime_error("Loop interval must be greater than or equal to 0");
    }

    return data;
}

/// @brief          Loads the screen position data from the indicator config.xml
/// @param p_handle The p_handle to the config.xml file
/// @param p_path   The C-string pointer to the section containing x- and y-pos attributes
/// @return         A struct containing the screen position data
tScreenPosition IndicatorConfig::LoadScreenPos(void* p_handle, const char* p_path)
{
    float xPos = GfParmGetNum(p_handle, p_path, PRM_ATTR_XPOS, nullptr, 0);
    float yPos = GfParmGetNum(p_handle, p_path, PRM_ATTR_YPOS, nullptr, 0);

    // Check whether x- and y-pos are valid percentages in range [0,1]
    if (xPos < 0.0f || yPos < 0.0f || xPos > 1.0f || yPos > 1.0f)
    {
        throw std::out_of_range("X and Y positions should be in the range [0,1]");
    }
    return {xPos, yPos};
}

/// @brief          Loads the texture indicator data from the indicator config.xml
/// @param p_handle The p_handle to the config.xml file
/// @param p_path   The p_path to the current intervention action to load
/// @return         The pointer to struct containing the texture data
tTextureData* IndicatorConfig::LoadTexture(void* p_handle, std::string p_path, InterventionType p_interventionType)
{
    p_path += PRM_SECT_TEXTURE;
    p_path += std::to_string(p_interventionType);
    if (!GfParmExistsSection(p_handle, p_path.c_str())) return nullptr;

    tTextureData* data = new TextureData;
    data->Path = GfParmGetStr(p_handle, p_path.c_str(), PRM_ATTR_SRC, "");
    data->ScrPos = LoadScreenPos(p_handle, p_path.c_str());
    return data;
}

/// @brief          Loads the text indicator data from the indicator config.xml
/// @param p_handle The p_handle to the config.xml file
/// @param p_path   The p_path to the current intervention action to load
/// @return         The pointer to struct containing the text data
tTextData* IndicatorConfig::LoadText(void* p_handle, std::string p_path)
{
    p_path += PRM_SECT_TEXT;
    if (!GfParmExistsSection(p_handle, p_path.c_str())) return nullptr;

    tTextData* data = new TextData;
    data->Text = GfParmGetStr(p_handle, p_path.c_str(), PRM_ATTR_CONTENT, "");
    data->ScrPos = LoadScreenPos(p_handle, p_path.c_str());

    const char* fontFile = GfParmGetStr(p_handle, p_path.c_str(), PRM_ATTR_FONT, "");
    int fontSize = (int)GfParmGetNum(p_handle, p_path.c_str(), PRM_ATTR_FONT_SIZE, nullptr, 10.0);

    char path[PATH_BUF_SIZE];
    snprintf(path, PATH_BUF_SIZE, "%sdata/fonts/%s", GfDataDir(), fontFile);
    data->Font = new GfuiFontClass(path);
    data->Font->create(fontSize);

    return data;
}

/// @brief  Returns an the existing IndicatorConfig, or creates the initial one if it doesn't exist yet.
/// @return The IndicatorConfig instance
IndicatorConfig* IndicatorConfig::GetInstance()
{
    // If the instance exists, return it.
    // Otherwise create the instance and store it for future calls.
    if (m_instance) return m_instance;

    // Check if IndicatorConfig file exists
    struct stat info = {};

    std::experimental::filesystem::path path = SingletonsFilePath();
    path.append("IndicatorConfig");
    std::string pathstring = path.string();
    const char* filepath = pathstring.c_str();
    int err = stat(filepath, &info);
    if (err == -1)
    {
        // File does not exist -> create pointer
        m_instance = new IndicatorConfig();
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
    m_instance = (IndicatorConfig*)pointerValue;
    return m_instance;
}

IndicatorConfig* IndicatorConfig::m_instance = nullptr;
