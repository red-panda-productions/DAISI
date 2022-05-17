#include <tgfclient.h>
#include "guimenu.h"
#include "legacymenu.h"
#include "Mediator.h"
#include "DataSelectionMenu.h"
#include "DataCompressionMenu.h"

#define PRM_DATA_COMPRESSION  "DataCompressionRadioButtonList"
#define PRM_CUSTOM_COMPRESSION_LEVEL "CustomCompressionLevelEdit"

// Names for the config file
#define RESEARCH_FILEPATH    "config/DataCompressionMenu.xml"
#define RESEARCH_SCREEN_NAME "DataCompressionMenu"

#define MAX_COMPRESSION_LEVEL               50

// GUI screen handles
static void* s_scrHandle = nullptr;
static void* s_prevHandle = nullptr;

// GUI settings Id's
int m_compressionLevelControl;

// InterventionType
DataCompressionType m_dataCompressionType;

// Custom compression level
int m_customCompressionLevel;
int m_customCompressionLevelControl;

// Apply Button
int m_applyButton;

/// @brief        Sets the compressionLevel to the selected one
/// @param p_info Information on the radio button pressed
static void SelectInterventionType(tRadioButtonInfo* p_info)
{
    m_dataCompressionType = (DataCompressionType)p_info->Selected;
}

/// @brief Handle input in the custom compression type textbox
static void SetMaxTime(void*)
{
    char* val = GfuiEditboxGetString(s_scrHandle, m_customCompressionLevelControl);
    char* endptr;
    m_customCompressionLevel = (int)strtol(val, &endptr, 0);
    if (*endptr != '\0')
        std::cerr << "Could not convert " << val << " to long and leftover string is: " << endptr << std::endl;

    if (m_customCompressionLevel > MAX_COMPRESSION_LEVEL)
        m_customCompressionLevel = MAX_COMPRESSION_LEVEL;
    else if (m_customCompressionLevel < 1)
        m_customCompressionLevel = 1;

    char buf[32];
    sprintf(buf, "%d", m_customCompressionLevel);
    GfuiEditboxSetString(s_scrHandle, m_customCompressionLevelControl, buf);
}

/// @brief Saves the settings into the DataCompressionMenu.xml file
static void SaveSettingsToDisk()
{
    // Copies xml to documents folder and then opens file parameter
    std::string dstStr(RESEARCH_FILEPATH);
    char dst[512];
    sprintf(dst, "%s%s", GfLocalDir(), dstStr.c_str());
    void* readParam = GfParmReadFile(dst, GFPARM_RMODE_REREAD | GFPARM_RMODE_CREAT);

    // Save compression level settings to xml file
    char val[32];
    sprintf(val, "%d", m_dataCompressionType);
    GfParmSetStr(readParam, PRM_DATA_COMPRESSION, GFMNU_ATTR_SELECTED, val);

    // Save custom compression level to xml file
    char buf[32];
    sprintf(buf, "%d", m_customCompressionLevel);
    GfParmSetStr(readParam, PRM_CUSTOM_COMPRESSION_LEVEL, GFMNU_ATTR_TEXT, buf);

    // Write all the above queued changed to xml file
    GfParmWriteFile(nullptr, readParam, RESEARCH_SCREEN_NAME);
}

/// @brief Saves the settings into the frontend settings and the backend config
static void SaveSettings(void* /* dummy */)
{
    if (!m_blackBoxChosen)
    {
        GfuiButtonSetText(s_scrHandle, m_applyButton, MSG_APPLY_NO_BLACK_BOX);
        return;
    }
    // Save settings to the SDAConfig
    SMediator* mediator = SMediator::GetInstance();
    mediator->SetAllowedActions(m_allowedActions);
    mediator->SetIndicatorSettings(m_indicators);
    mediator->SetInterventionType(m_interventionType);
    mediator->SetMaxTime(m_maxTime);
    mediator->SetPControlSettings(m_pControl);
    mediator->SetBlackBoxFilePath(m_blackBoxFilePath);

    // Save settings in the DataCompressionMenu.xml
    SaveSettingsToDisk();

    // Go to the next screen
    GfuiScreenActivate(s_nextHandle);
}
