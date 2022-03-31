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

#define PRM_ATTR_XPOS           "xpos"
#define PRM_ATTR_YPOS           "ypos"
#define PRM_ATTR_SRC            "source"

static std::unordered_map<InterventionAction, const char*> s_actionEnumParamMap = {
    { INTERVENTION_ACTION_NONE,       "none"        },
    { INTERVENTION_ACTION_TURN_LEFT,  "steer left"  },
    { INTERVENTION_ACTION_TURN_RIGHT, "steer right" },
    { INTERVENTION_ACTION_BRAKE,      "brake"       }
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

/// @brief Represents the configuration of interventions
class InterventionConfig 
{
public:
    void Initialize();

    void* GetXmlHandle();

    void SetInterventionAction(InterventionAction p_action);

    void SetTextures(tTextureData* p_textures);

    tTextureData GetCurrentInterventionTexture();

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
    std::unordered_map<InterventionAction, const char*> m_sounds;
};
