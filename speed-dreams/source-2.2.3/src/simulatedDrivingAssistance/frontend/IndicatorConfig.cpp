#include <fstream>
#include <string>
#include <portability.h>
#include <stdexcept>
#include <tgf.h>
#include "IndicatorConfig.h"

/// @brief        Loads the sound indicator data from the indicator config.xml
/// @param handle The handle to the config.xml file
/// @param path   The path to the current intervention action to load
/// @return       The pointer to the struct containing the sound data
tSoundData* LoadSound(void* handle, std::string path)
{
    path += PRM_SECT_SOUND;
    if (!GfParmExistsSection(handle, path.c_str())) return nullptr;

    tSoundData* data = new SoundData;

    const char* source = GfParmGetStr(handle, path.c_str(), PRM_ATTR_SRC, "");

    char* sndPath = new char[PATH_BUF_SIZE];
    snprintf(sndPath, PATH_BUF_SIZE, SOUNDS_DIR_FORMAT, GfDataDir(), source);
    data->Path = sndPath;

    return data;
}

/// @brief        Loads the screen position data from the indicator config.xml
/// @param handle The handle to the config.xml file
/// @param path   The C-string pointer to the section containing x- and y-pos attributes
/// @return       A struct containing the screen position data
tScreenPosition LoadScreenPos(void* handle, const char* path)
{
    float xPos = GfParmGetNum(handle, path, PRM_ATTR_XPOS, NULL, 0);
    float yPos = GfParmGetNum(handle, path, PRM_ATTR_YPOS, NULL, 0);
    return { xPos, yPos };
}

/// @brief        Loads the texture indicator data from the indicator config.xml
/// @param handle The handle to the config.xml file
/// @param path   The path to the current intervention action to load
/// @return       The pointer to struct containing the texture data
tTextureData* LoadTexture(void* handle, std::string path)
{
    path += PRM_SECT_TEXTURE;

    tTextureData* data = new TextureData;
    data->Path   = GfParmGetStr(handle, path.c_str(), PRM_ATTR_SRC, "");
    data->ScrPos = LoadScreenPos(handle, path.c_str());
    return data;
}

/// @brief        Loads the text indicator data from the indicator config.xml
/// @param handle The handle to the config.xml file
/// @param path   The path to the current intervention action to load
/// @return       The pointer to struct containing the text data
tTextData* LoadText(void* handle, std::string path)
{
    path += PRM_SECT_TEXT;

    tTextData* data = new TextData;
    data->Text   = GfParmGetStr(handle, path.c_str(), PRM_ATTR_CONTENT, "");
    data->ScrPos = LoadScreenPos(handle, path.c_str());
    return data;
}

/// @brief Loads the indicator data of every intervention action from the config.xml file.
void IndicatorConfig::LoadIndicatorData()
{
    // Load intervention texture from XML file (unchecked max path size: 256)
    char path[PATH_BUF_SIZE];
    snprintf(path, PATH_BUF_SIZE, CONFIG_XML_DIR_FORMAT, GfDataDir());
    void* xmlHandle =  GfParmReadFile(path, GFPARM_RMODE_STD);

    int count = GfParmGetEltNb(xmlHandle, PRM_SECT_INTERVENTIONS);

    // Load the indicator data for every intervention action
    m_indicatorData = std::vector<tIndicatorData>(count);
    for (int i = 0; i < count; i++)
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

std::vector<tIndicatorData> IndicatorConfig::GetIndicatorData()
{
    return m_indicatorData;
}

std::vector<tIndicatorData> IndicatorConfig::GetActiveIndicators()
{
    return { m_indicatorData[1], m_indicatorData[2] };
}

/// @brief Initialize the intervention configuration.
void IndicatorConfig::Initialize() {
    LoadIndicatorData();
}

/// @brief          Sets the current active intervention action
/// @param p_action The intervention to set as current active action
void IndicatorConfig::ActivateIndicator(InterventionAction p_action) 
{
    m_activeIndicators = { m_indicatorData[p_action] };
}

/// @brief  Returns an the existing IndicatorConfig, or creates the initial one if it doesn't exist yet.
/// @return The IndicatorConfig instance
IndicatorConfig* IndicatorConfig::GetInstance() 
{
    if (m_instance == nullptr)
    {
        // check if IndicatorConfig file exists
        struct stat info;
        char workingDir[256];
        getcwd(workingDir, 256);
        std::string workingDirectory(workingDir);
        workingDirectory += "\\Singletons\\IndicatorConfig";
        const char* filepath = workingDirectory.c_str();
        int err = stat(filepath, &info);
        if (err == -1)
        {
            // file does not exist create pointer
            m_instance = new IndicatorConfig();
            std::ofstream file("Singletons/IndicatorConfig");
            file << m_instance;
            file.close();
            m_instance->Initialize();
            return m_instance;
        }

        // file exists read pointer
        std::string pointerName("00000000");
        std::ifstream file("Singletons/IndicatorConfig");
        getline(file, pointerName);
        file.close();
        int pointerValue = stoi(pointerName, 0, 16);
        m_instance = (IndicatorConfig*)pointerValue;
    }
    return m_instance;
}

IndicatorConfig* IndicatorConfig::m_instance = nullptr;
