#pragma once

#include <plib/ssg.h>
#include "ConfigEnums.h"

// Location of the intervention XML file, with respect to the root data directory.
#define INTERVENTION_DATA_DIR_FORMAT "%sdata/intervention/intervention.xml"

// Parameters of sections and attributes used to search in the XML file.
#define PRM_SECT_INTERVENTIONS  "Interventions"
#define PRM_ATTR_NAME           "name"
#define PRM_ATTR_TEXTURE        "texture"
#define PRM_ATTR_XPOS           "xpos"
#define PRM_ATTR_YPOS           "ypos"


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

    InterventionAction m_currentAction;
    tTextureData* m_textures;
};
