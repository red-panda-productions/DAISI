#pragma once

#include <plib/ssg.h>
#include "ConfigEnums.h"
#include <unordered_map>

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

/// @brief Represents a position on screen
typedef struct ScreenPosition 
{
    int X, Y;
} tScreenPosition;

/// @brief Stores all data of a texture
typedef struct TextureData 
{
    ssgSimpleState* Texture;
    tScreenPosition Position;

    TextureData() = default;
    TextureData(ssgSimpleState* p_tex, tScreenPosition p_pos)
        : Texture(p_tex) , Position(p_pos) { }
} tTextureData;

/// @brief Stores data for text
typedef struct TextData
{
    const char*     Text;
    tScreenPosition Position;
} tTextData;

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
    tTextureData* m_textures;
    tTextData* m_texts;
    std::unordered_map<InterventionAction, const char*> m_sounds;
};
