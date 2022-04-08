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
class IndicatorConfig 
{
public:
    void Initialize();

    void ActivateIndicator(InterventionAction p_action);

    std::vector<tIndicatorData> GetIndicatorData();

    std::vector<tIndicatorData> GetActiveIndicators();


    static IndicatorConfig* GetInstance();

    /// @brief Removes copy constructor for singleton behaviour
    IndicatorConfig(IndicatorConfig const&) = delete;

    /// @brief Removes assigment for singleton behaviour
    void operator=(IndicatorConfig const&) = delete;

 private:
    IndicatorConfig() = default;
    static IndicatorConfig* m_instance;

    InterventionAction m_currentAction = 1;

    std::vector<tIndicatorData> m_indicatorData;

    std::vector<tIndicatorData> m_activeIndicators;

    void LoadIndicatorData();

};
