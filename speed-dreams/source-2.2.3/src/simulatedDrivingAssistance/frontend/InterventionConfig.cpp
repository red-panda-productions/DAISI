#include <fstream>
#include <string>
#include <portability.h>
#include <stdexcept>

#include "InterventionConfig.h"

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
    if (m_currentAction >= sizeof(m_textures))
    {
        throw std::out_of_range("Intervention index (Enum) is out-of-bounds of textures array");
    }
    return m_textures[m_currentAction];
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