#include <fstream>
#include <string>
#include <portability.h>
#include <stdexcept>
#include <tgf.h>

#include "InterventionConfig.h"

void InterventionConfig::Initialize() {
    void* xmlHandle = GetXmlHandle();

    char interventionPath[256];
    snprintf(interventionPath, sizeof(interventionPath), PRM_SECT_INTERVENTIONS);
    m_interventionCount = GfParmGetEltNb(xmlHandle, interventionPath);
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
    return {
        {INTERVENTION_ACTION_TURN_LEFT, "data/sound/interventions/left.wav"},
        {INTERVENTION_ACTION_TURN_RIGHT, "data/sound/interventions/right.wav"},
        {INTERVENTION_ACTION_BRAKE, "data/sound/interventions/break.wav"}
    };
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
