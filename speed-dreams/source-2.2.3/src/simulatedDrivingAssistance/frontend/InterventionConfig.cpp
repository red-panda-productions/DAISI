#include <fstream>
#include <string>
#include <portability.h>
#include <stdexcept>
#include <tgf.h>
#include "InterventionConfig.h"

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
void InterventionConfig::LoadIndicatorData()
{
    // Load intervention texture from XML file (unchecked max path size: 256)
    char path[PATH_BUF_SIZE];
    snprintf(path, PATH_BUF_SIZE, CONFIG_XML_DIR_FORMAT, GfDataDir());
    void* xmlHandle =  GfParmReadFile(path, GFPARM_RMODE_STD);

    // Get the number of defined interventions from the xml file and store this for re-use.
    m_interventionCount = GfParmGetEltNb(xmlHandle, PRM_SECT_INTERVENTIONS);

    // Load the indicator data for every intervention action
    m_indicators = std::vector<tIndicatorData>(m_interventionCount);
    for (int i = 0; i < m_interventionCount; i++)
    {
        snprintf(path, PATH_BUF_SIZE, "%s/%s/", PRM_SECT_INTERVENTIONS, s_actionEnumString[i]);
        m_indicators[i] = { 
            LoadSound(xmlHandle, std::string(path)),
            LoadTexture(xmlHandle, std::string(path)),
            LoadText(xmlHandle, std::string(path))
        };
    }
}

/// @brief Initialize the intervention sound map. Note that this just gets which interventions use which sounds and does not create the sounds yet.
/// @param p_sounds The map in which to store the sounds
/// @param p_xmlHandle The handle to the xml config file to read from
/// @param p_interventionCount The amount of interventions
void InitializeSounds(std::unordered_map<InterventionAction, const char*>& p_sounds, void* p_xmlHandle, unsigned int p_interventionCount) {
    char path[256];
    for (int i = 0; i < p_interventionCount; i++)
    {
        snprintf(path, PATH_BUF_SIZE, "%s/%s/%s", PRM_SECT_INTERVENTIONS, s_actionEnumString[i], PRM_SECT_SOUND);
        if(!GfParmExistsSection(p_xmlHandle, path)) continue;

        const char* src = GfParmGetStr(p_xmlHandle, path, PRM_ATTR_SRC, "");
        char* soundPath = new char[256];
        snprintf(soundPath, 256, SOUNDS_DIR_FORMAT, GfDataDir(), src);

        p_sounds[i] = soundPath;
    }
}

std::vector<tIndicatorData> InterventionConfig::GetIndicators()
{
    return m_indicators;
}

/// @brief Initialize the intervention configuration.
void InterventionConfig::Initialize() {
    LoadIndicatorData();
}

/// @brief  Get the XML file for the intervention configuration
/// @return An XML handle for the intervention configuration
void* InterventionConfig::GetXmlHandle() {
    // Load intervention texture from XML file (unchecked max path size: 256)
    char* path = new char[256];
    snprintf(path, 256, CONFIG_XML_DIR_FORMAT, GfDataDir());
    return GfParmReadFile(path, GFPARM_RMODE_STD);
}

/// @brief          Sets the current active intervention action
/// @param p_action The intervention to set as current active action
void InterventionConfig::SetInterventionAction(InterventionAction p_action) 
{
    m_currentAction = p_action;
}

/// @brief              Sets the textures that are used by the HUD
/// @param p_textures   An array containing the texture data, 
///                     indexed by the InterventionAction type in ConfigEnums.h
void InterventionConfig::SetTextures(tTextureData* p_textures) 
{
    //m_textures = p_textures;
}

/// @brief  Retrieves the texture belonging to the current intervention action
/// @return The texture data
tTextureData InterventionConfig::GetCurrentInterventionTexture() 
{
    /*if (m_currentAction >= m_interventionCount)
    {
        throw std::out_of_range("Intervention index (Enum) is out-of-bounds of textures array");
    }
    return m_textures[m_currentAction];*/
    return {};
}

/// @brief              Sets the texts that are used by the HUD
/// @param p_textures   An array containing the text data, 
///                     indexed by the InterventionAction type in ConfigEnums.h
void InterventionConfig::SetTexts(tTextData* p_texts)
{
    //m_texts = p_texts;
}

/// @brief  Retrieves the text belonging to the current intervention action
/// @return The text data
tTextData InterventionConfig::GetCurrentInterventionText()
{
    /*if (m_currentAction >= m_interventionCount)
    {
        throw std::out_of_range("Intervention index (Enum) is out-of-bounds of texts array");
    }
    return m_texts[m_currentAction];*/
    return {};
}

/// @brief  Retrieves the sound locations belonging to the possible intervention actions
/// @return A map going from InterventionAction => sound location, possibly nullptr
std::unordered_map<InterventionAction, const char*> InterventionConfig::GetSounds() {
    return m_sounds;
}

/// @brief  Retrieves the intervention actions that should be playing a sound effect.
/// @return A vector containing the intervention actions that should play a sound effect.
std::vector<InterventionAction> InterventionConfig::GetEnabledSounds() {
    return { m_currentAction };
}

/// @brief  Gets the amount of interventions
/// @return The amount of interventions
unsigned int InterventionConfig::GetInterventionCount() const {
    return m_interventionCount;
}

/// @brief  Returns an the existing InterventionConfig, or creates the initial one if it doesn't exist yet.
/// @return The InterventionConfig instance
InterventionConfig* InterventionConfig::GetInstance() 
{
    if (m_instance == nullptr)
    {
        // check if InterventionConfig file exists
        struct stat info;
        char workingDir[256];
        getcwd(workingDir, 256);
        std::string workingDirectory(workingDir);
        workingDirectory += "\\Singletons\\InterventionConfig";
        const char* filepath = workingDirectory.c_str();
        int err = stat(filepath, &info);
        if (err == -1)
        {
            // file does not exist create pointer
            m_instance = new InterventionConfig();
            std::ofstream file("Singletons/InterventionConfig");
            file << m_instance;
            file.close();
            m_instance->Initialize();
            return m_instance;
        }

        // file exists read pointer
        std::string pointerName("00000000");
        std::ifstream file("Singletons/InterventionConfig");
        getline(file, pointerName);
        file.close();
        int pointerValue = stoi(pointerName, 0, 16);
        m_instance = (InterventionConfig*)pointerValue;
    }
    return m_instance;
}

InterventionConfig* InterventionConfig::m_instance = nullptr;
