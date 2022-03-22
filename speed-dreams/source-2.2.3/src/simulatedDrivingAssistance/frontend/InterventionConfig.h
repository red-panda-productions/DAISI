#pragma once

#include <plib/ssg.h>
#include "ConfigEnums.h"
#include <unordered_map>

// Location of the intervention XML file, with respect to the root data directory.
#define INTERVENTION_DATA_DIR_FORMAT "%sdata/intervention/intervention.xml"
#define SOUNDS_DIR_FORMAT "%sdata/intervention/sound/%s"

// Parameters of sections and attributes used to search in the XML file.
#define PRM_SECT_INTERVENTIONS  "Interventions"
#define PRM_SECT_SOUND          "sound"
#define PRM_ATTR_NAME           "name"
#define PRM_ATTR_TEXTURE        "texture"
#define PRM_ATTR_XPOS           "xpos"
#define PRM_ATTR_YPOS           "ypos"
#define PRM_ATTR_SRC            "source"


typedef struct ScreenPosition 
{
    int X, Y;
} tScreenPosition;


typedef struct TextureData 
{
    const char* Name;
    ssgSimpleState* Texture;
    tScreenPosition Position;

    TextureData() = default;
    TextureData(const char* p_name, ssgSimpleState* p_tex, tScreenPosition p_pos)
        : Name(p_name) , Texture(p_tex) , Position(p_pos) { }
} tTextureData;


class InterventionConfig 
{
public:

    /// @brief Initialize the intervention configuration.
    void Initialize();

    void* GetXmlHandle();

    /// @brief          Sets the current active intervention action
    /// @param p_action The intervention to set as current active action
    void SetInterventionAction(InterventionAction p_action);

    /// @brief              Sets the textures that are used by the HUD
    /// @param p_textures   An array containing the texture data, 
    ///                     indexed by the InterventionAction type in ConfigEnums.h
    void SetTextures(tTextureData* p_textures);

    /// @brief  Retrieves the texture belonging to the current intervention action
    /// @return The texture data
    tTextureData GetCurrentInterventionTexture();

    /// @brief  Retrieves the sound locations belonging to the possible intervention actions
    /// @return A map going from InterventionAction => sound location, possibly nullptr
    std::unordered_map<InterventionAction, const char*> GetSounds();

    /// @brief  Retrieves the intervention actions that should be playing a sound effect.
    /// @return A vector containing the intervention actions that should play a sound effect.
    std::vector<InterventionAction> GetEnabledSounds();

    unsigned int GetInterventionCount();

    /// @brief  Returns an the existing InterventionConfig, or creates the initial one if it doesn't exist yet.
    /// @return The InterventionConfig instance
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
