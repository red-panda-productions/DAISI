#pragma once

#include <plib/ssg.h>
#include "ConfigEnums.h"
#include <unordered_map>

// Location of the intervention XML file, with respect to the root data directory.
#define INTERVENTION_DATA_DIR_FORMAT "%sdata/intervention/intervention.xml"

// Parameters of sections and attributes used to search in the XML file.
#define PRM_SECT_INTERVENTIONS  "Interventions"
#define PRM_ATTR_NAME           "name"
#define PRM_ATTR_TEXTURE        "texture"
#define PRM_ATTR_XPOS           "xpos"
#define PRM_ATTR_YPOS           "ypos"

/// @brief Represents a position on screen
typedef struct ScreenPosition 
{
    int X, Y;
} tScreenPosition;

/// @brief Stores all data of a texture
typedef struct TextureData 
{
    const char* Name;
    ssgSimpleState* Texture;
    tScreenPosition Position;

    TextureData() = default;
    TextureData(const char* p_name, ssgSimpleState* p_tex, tScreenPosition p_pos)
        : Name(p_name) , Texture(p_tex) , Position(p_pos) { }
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

    unsigned int GetInterventionCount();

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
};
