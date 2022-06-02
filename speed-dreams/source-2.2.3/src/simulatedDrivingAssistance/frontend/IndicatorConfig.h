#pragma once
#include <vector>
#include <string>
#include "ConfigEnums.h"
#include "IndicatorData.h"

// Size of the path buffers, this will be used unchecked.
#define PATH_BUF_SIZE 256

// Location of the config.xml file with respect to the root data directory.
#define CONFIG_XML_DIR_FORMAT "%sdata/indicators/Config.xml"
#define SOUNDS_DIR_FORMAT     "%sdata/indicators/sound/%s"

// Parameters of sections and attributes used to search in the XML file.
#define PRM_SECT_INDICATORS "indicators"
#define PRM_SECT_TEXTURES   "textures"
#define PRM_SECT_SOUND      "sound"
#define PRM_SECT_TEXT       "text"

#define PRM_ATTR_XPOS          "xpos"
#define PRM_ATTR_YPOS          "ypos"
#define PRM_ATTR_WIDTH         "width"
#define PRM_ATTR_HEIGHT        "height"
#define PRM_ATTR_SRC           "source"
#define PRM_ATTR_CONTENT       "content"
#define PRM_ATTR_FONT          "font"
#define PRM_ATTR_FONT_SIZE     "font-size"
#define PRM_ATTR_LOOPING       "looping"
#define PRM_ATTR_LOOP_INTERVAL "loop-interval"

#define VAL_YES "yes"
#define VAL_NO  "no"

static const char* s_interventionActionString[NUM_INTERVENTION_ACTION] = {
    "steer neutral", "steer left", "steer right", "steer straight",
    "speed neutral", "accelerate", "brake"};

static const char* s_interventionTypeString[NUM_INTERVENTION_TYPES] = {
    "no-help", "signals-only", "shared-control", "complete-takeover", "autonomous-ai"};

/// @brief Contains the configuration of indicators for interventions
class IndicatorConfig
{
public:
    void LoadIndicatorData(const char* p_path, InterventionType p_interventionType);

    void ActivateIndicator(InterventionAction p_action);

    void ResetActiveIndicatorsToNeutral();

    std::vector<tIndicatorData> GetIndicatorData();

    std::vector<tIndicatorData> GetActiveIndicators();

    static IndicatorConfig* GetInstance();

#ifdef TEST
    static void ClearInstance()
    {
        delete m_instance;
        m_instance = nullptr;
    }
#endif

    /// @brief Removes copy constructor for singleton behaviour
    IndicatorConfig(IndicatorConfig const&) = delete;

    /// @brief Removes assigment for singleton behaviour
    void operator=(IndicatorConfig const&) = delete;

private:
    IndicatorConfig() = default;
    static IndicatorConfig* m_instance;

    // A vector containing indicator data of every single intervention action, including the neutral actions.
    std::vector<tIndicatorData> m_indicatorData = std::vector<tIndicatorData>(NUM_INTERVENTION_ACTION);

    // A vector containing the active indicator per intervention action type, indexed by an IndicatorActionType.
    std::vector<tIndicatorData> m_activeIndicators = std::vector<tIndicatorData>(NUM_INTERVENTION_ACTION_TYPES);

    // Loading helpers
    tSoundData* LoadSound(void* p_handle, std::string p_path);
    tScreenPosition LoadScreenPos(void* p_handle, const char* p_path);
    tTextureDimensions LoadDimensions(void* p_handle, const char* p_path);
    tTextureData* LoadTexture(void* p_handle, std::string p_path, InterventionType p_interventionType);
    tTextData* LoadText(void* p_handle, std::string p_path);
};