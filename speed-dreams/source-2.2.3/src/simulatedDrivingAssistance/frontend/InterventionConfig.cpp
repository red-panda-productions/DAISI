#include <fstream>
#include <string>
#include <portability.h>
#include <stdexcept>
#include <tgf.h>

#include "InterventionConfig.h"

void InitializeSounds(std::unordered_map<InterventionAction, const char*>& p_sounds, void* p_xmlHandle, unsigned int p_interventionCount) {
    char path[256];
    for (int i = 0; i < p_interventionCount; i++)
    {
        snprintf(path, sizeof(path), "%s/%d/%s", PRM_SECT_INTERVENTIONS, i, PRM_SECT_SOUND);
        if(!GfParmExistsSection(p_xmlHandle, path)) continue;

        const char* src = GfParmGetStr(p_xmlHandle, path, PRM_ATTR_SRC, "");
        char* soundPath = new char[256];
        snprintf(soundPath, 256, SOUNDS_DIR_FORMAT, GfDataDir(), src);

        p_sounds[i] = soundPath;
    }
}

void InterventionConfig::Initialize() {
    void* xmlHandle = GetXmlHandle();

    char interventionPath[256];
    snprintf(interventionPath, sizeof(interventionPath), PRM_SECT_INTERVENTIONS);
    m_interventionCount = GfParmGetEltNb(xmlHandle, interventionPath);

    InitializeSounds(m_sounds, xmlHandle, m_interventionCount);
}

void* InterventionConfig::GetXmlHandle() {
    // Load intervention texture from XML file (unchecked max path size: 256)
    char* path = new char[256];
    snprintf(path, 256, INTERVENTION_DATA_DIR_FORMAT, GfDataDir());
    return GfParmReadFile(path, GFPARM_RMODE_STD);
}

void InterventionConfig::SetInterventionAction(InterventionAction p_action) 
{
    m_currentAction = p_action;
}

void InterventionConfig::SetTextures(tTextureData* p_textures) 
{
    m_textures = p_textures;
}

tTextureData InterventionConfig::GetCurrentInterventionTexture() 
{
    if (m_currentAction >= m_interventionCount)
    {
        throw std::out_of_range("Intervention index (Enum) is out-of-bounds of textures array");
    }
    return m_textures[m_currentAction];
}

std::unordered_map<InterventionAction, const char*> InterventionConfig::GetSounds() {
    return m_sounds;
}

std::vector<InterventionAction> InterventionConfig::GetEnabledSounds() {
    return { m_currentAction };
}

unsigned int InterventionConfig::GetInterventionCount() {
    return m_interventionCount;
}

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
