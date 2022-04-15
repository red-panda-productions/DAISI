#include <fstream>
#include <string>
#include <portability.h>
#include <stdexcept>
#include <tgf.h>

#include "IndicatorConfig.h"

/// @brief Initialize the indicator configuration.
void IndicatorConfig::Initialize()
{
    LoadIndicatorData();
}

/// @brief  Returns a vector of ALL indicator data
/// @return The vector of indicator data
std::vector<tIndicatorData> IndicatorConfig::GetIndicatorData()
{
    return m_indicatorData;
}

/// @brief  Returns a vector containing indicator data for all active indicators
/// @return The vector of indicator data
std::vector<tIndicatorData> IndicatorConfig::GetActiveIndicators()
{
    return m_activeIndicators;
}


/// @brief          Activates the given intervention indicator
/// @param p_action The intervention to activate the indicators for
void IndicatorConfig::ActivateIndicator(InterventionAction p_action)
{
    // TODO: add to the vector instead of overwriting it, this also requires 
    //       a way to remove the indicator after some time has passed.
    m_activeIndicators = { m_indicatorData[p_action] };
}

/// @brief Loads the indicator data of every intervention action from the config.xml file.
void IndicatorConfig::LoadIndicatorData()
{
    // Load intervention indicator texture from XML file (unchecked max p_path size: 256)
    char path[PATH_BUF_SIZE];
    snprintf(path, PATH_BUF_SIZE, CONFIG_XML_DIR_FORMAT, GfDataDir());
    void* xmlHandle =  GfParmReadFile(path, GFPARM_RMODE_STD);

    // Load the indicator data for every intervention action
    m_indicatorData = std::vector<tIndicatorData>(NUM_INTERVENTION_ACTION);
    for (int i = 0; i < NUM_INTERVENTION_ACTION; i++)
    {
        snprintf(path, PATH_BUF_SIZE, "%s/%s/", PRM_SECT_INTERVENTIONS, s_actionEnumString[i]);
        m_indicatorData[i] = { 
            (InterventionAction)i,
            LoadSound(xmlHandle, std::string(path)),
            LoadTexture(xmlHandle, std::string(path)),
            LoadText(xmlHandle, std::string(path))
        };
    }
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

    if(data->LoopInterval < 0) {
        throw std::runtime_error("Loop interval must be greater than 0");
    }

    return data;
}

/// @brief          Loads the screen position data from the indicator config.xml
/// @param p_handle The p_handle to the config.xml file
/// @param p_path   The C-string pointer to the section containing x- and y-pos attributes
/// @return         A struct containing the screen position data
tScreenPosition IndicatorConfig::LoadScreenPos(void* p_handle, const char* p_path)
{
    float xPos = GfParmGetNum(p_handle, p_path, PRM_ATTR_XPOS, NULL, 0);
    float yPos = GfParmGetNum(p_handle, p_path, PRM_ATTR_YPOS, NULL, 0);

    // Check whether x- and y-pos are valid percentages in range [0,1]
    if (xPos < 0.0f || yPos < 0.0f || xPos > 1.0f || yPos > 1.0f)
    {
        throw std::out_of_range("X and Y positions should be in the range [0,1]");
    }
    return { xPos, yPos };
}

/// @brief          Loads the texture indicator data from the indicator config.xml
/// @param p_handle The p_handle to the config.xml file
/// @param p_path   The p_path to the current intervention action to load
/// @return         The pointer to struct containing the texture data
tTextureData* IndicatorConfig::LoadTexture(void* p_handle, std::string p_path)
{
    p_path += PRM_SECT_TEXTURE;
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
    struct stat info;
    char workingDir[256];
    getcwd(workingDir, 256);
    std::string workingDirectory(workingDir);
    workingDirectory += "\\Singletons\\IndicatorConfig";
    const char* filepath = workingDirectory.c_str();
    int err = stat(filepath, &info);
    if (err == -1)
    {
        // File does not exist -> create pointer
        m_instance = new IndicatorConfig();
        std::ofstream file("Singletons/IndicatorConfig");
        file << m_instance;
        file.close();
        m_instance->Initialize();
        return m_instance;
    }

    // File exists -> read pointer
    std::string pointerName("00000000");
    std::ifstream file("Singletons/IndicatorConfig");
    getline(file, pointerName);
    file.close();
    int pointerValue = stoi(pointerName, 0, 16);
    m_instance = (IndicatorConfig*)pointerValue;
    return m_instance;
}

IndicatorConfig* IndicatorConfig::m_instance = nullptr;
