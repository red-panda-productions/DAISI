#pragma once
#include <vector>
#include <string>
#include "ConfigEnums.h"
#include "IndicatorData.h"

// Size of the path buffers, this will be used unchecked.
#define PATH_BUF_SIZE 256

// Location of the config.xml file with respect to the root data directory.
#define CONFIG_XML_DIR_FORMAT "%sdata/indicators/config.xml"
#define SOUNDS_DIR_FORMAT     "%sdata/indicators/sound/%s"

// Parameters of sections and attributes used to search in the XML file.
#define PRM_SECT_INTERVENTIONS "Interventions"
#define PRM_SECT_TEXTURE       "texture"
#define PRM_SECT_SOUND         "sound"
#define PRM_SECT_TEXT          "text"

#define PRM_ATTR_XPOS          "xpos"
#define PRM_ATTR_YPOS          "ypos"
#define PRM_ATTR_SRC           "source"
#define PRM_ATTR_CONTENT       "content"
#define PRM_ATTR_FONT          "font"
#define PRM_ATTR_FONT_SIZE     "font-size"
#define PRM_ATTR_LOOPING       "looping"
#define PRM_ATTR_LOOP_INTERVAL "loop-interval"

#define VAL_YES "yes"
#define VAL_NO  "no"
static const char* s_actionEnumString[NUM_INTERVENTION_ACTION] = {
    "none", "steer left", "steer right", "brake", "accelerate"};

/// @brief Contains the configuration of indicators for interventions
class IndicatorConfig
{
public:
    void LoadIndicatorData(const char* p_path);

    void ActivateIndicator(InterventionAction p_action);

    std::vector<tIndicatorData> GetIndicatorData();

    std::vector<tIndicatorData> GetActiveIndicators(InterventionType p_interventionType);

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

    std::vector<tIndicatorData> m_indicatorData = std::vector<tIndicatorData>(NUM_INTERVENTION_ACTION);

    std::vector<tIndicatorData> m_activeIndicators = {};

    // Loading helpers
    tSoundData* LoadSound(void* p_handle, std::string p_path);
    tScreenPosition LoadScreenPos(void* p_handle, const char* p_path);
    tTextureData* LoadTexture(void* p_handle, std::string p_path);
    tTextData* LoadText(void* p_handle, std::string p_path);
};