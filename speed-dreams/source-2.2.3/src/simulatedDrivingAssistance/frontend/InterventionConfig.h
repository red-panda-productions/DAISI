#pragma once

#include <plib/ssg.h>
#include "ConfigEnums.h"
#include <unordered_map>
#include "IndicatorData.h"

// Size of the path buffers, this will be used unchecked.
#define PATH_BUF_SIZE 256

// Location of the config.xml file with respect to the root data directory.
#define CONFIG_XML_DIR_FORMAT "%sdata/intervention/config.xml"
#define SOUNDS_DIR_FORMAT     "%sdata/intervention/sound/%s"

// Parameters of sections and attributes used to search in the XML file.
#define PRM_SECT_INTERVENTIONS  "Interventions"
#define PRM_SECT_TEXTURE        "texture"
#define PRM_SECT_SOUND          "sound"
#define PRM_SECT_TEXT           "text"

#define PRM_ATTR_XPOS           "xpos"
#define PRM_ATTR_YPOS           "ypos"
#define PRM_ATTR_SRC            "source"
#define PRM_ATTR_CONTENT        "content"

static const char* s_actionEnumString[NUM_INTERVENTION_ACTION] = {
    "none", "steer left", "steer right", "brake"
};


/// @brief Represents the configuration of interventions
class InterventionConfig 
{
public:
    void Initialize();

    void* GetXmlHandle();

    void SetInterventionAction(InterventionAction p_action);

    void SetTextures(tTextureData* p_textures);

    tTextureData GetCurrentInterventionTexture();

    void SetTexts(tTextData* p_texts);

    tTextData GetCurrentInterventionText();

    std::unordered_map<InterventionAction, const char*> GetSounds();

    std::vector<InterventionAction> GetEnabledSounds();

    std::vector<tIndicatorData> GetIndicators();


    void LoadIndicatorData();


    unsigned int GetInterventionCount() const;

    static InterventionConfig* GetInstance();

    /// @brief Removes copy constructor for singleton behaviour
    InterventionConfig(InterventionConfig const&) = delete;

    /// @brief Removes assigment for singleton behaviour
    void operator=(InterventionConfig const&) = delete;

 private:
    InterventionConfig() = default;
    static InterventionConfig* m_instance;

    unsigned int m_interventionCount = 0;
    InterventionAction m_currentAction;
    //tTextureData* m_textures;
    //tTextData* m_texts;
    std::unordered_map<InterventionAction, const char*> m_sounds;

    std::vector<tIndicatorData> m_indicators;
};
