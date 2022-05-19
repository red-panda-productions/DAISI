#include <tgfclient.h>
#include "guimenu.h"
#include "legacymenu.h"
#include "Mediator.h"
#include "ConfigEnums.h"
#include "DataCompressionMenu.h"

// Parameters used in the xml files
#define PRM_DATA_COMPRESSION   "DataCompressionRadioButtonList"
#define PRM_CUSTOM_COMPRESSION "CustomCompressionEdit"

// Names for the config file
#define COMPRESSION_FILEPATH    "config/DataCompressionMenu.xml"
#define COMPRESSION_SCREEN_NAME "DataCompressionMenu"

#define MAX_COMPRESSION_LEVEL 49

// GUI screen handles
static void* s_scrHandle = nullptr;
static void* s_prevHandle = nullptr;

// GUI settings Id's
int m_compressionButtonList;

// InterventionType
DataCompressionType m_dataCompressionType;

// Custom compression level
int m_customCompressionLevel;
int m_customCompressionLevelControl;

// Apply Button
int m_applyButtonComp;

/// @brief         Loads the settings from the config file
/// @param p_param A handle to the parameter file
static void LoadSettingsFromFile(void* p_param)
{
    m_dataCompressionType = std::stoi(GfParmGetStr(p_param, PRM_DATA_COMPRESSION, GFMNU_ATTR_SELECTED, "1"));
    m_customCompressionLevel = std::stoi(GfParmGetStr(p_param, PRM_CUSTOM_COMPRESSION, GFMNU_ATTR_TEXT, nullptr));
}

/// @brief Makes sure all visuals display the internal values
static void SynchronizeControls()
{
    GfuiRadioButtonListSetSelected(s_scrHandle, m_compressionButtonList, static_cast<int>(m_dataCompressionType));

    char buf[32];
    sprintf(buf, "%d", m_customCompressionLevel);
    GfuiEditboxSetString(s_scrHandle, m_customCompressionLevelControl, buf);
}

/// @brief Loads default settings
static void LoadDefaultSettings()
{
    m_dataCompressionType = GfuiRadioButtonListGetSelected(s_scrHandle, m_compressionButtonList);
    m_customCompressionLevel = std::stoi(GfuiEditboxGetString(s_scrHandle, m_customCompressionLevelControl));
}

/// @brief Loads (if possible) the settings; otherwise, the control's default settings will be used
static void LoadSettings()
{
    char buf[512];
    sprintf(buf, "%s%s", GfLocalDir(), COMPRESSION_FILEPATH);
    if (GfFileExists(buf))
    {
        void* param = GfParmReadFile(buf, GFPARM_RMODE_STD);
        // Initialize settings with the retrieved xml file
        LoadSettingsFromFile(param);
        SynchronizeControls();
        return;
    }
    LoadDefaultSettings();
}

/// @brief Saves the settings to a file
static void SaveSettingsToFile()
{
    std::string dstStr(COMPRESSION_FILEPATH);
    char dst[512];
    sprintf(dst, "%s%s", GfLocalDir(), dstStr.c_str());
    void* readParam = GfParmReadFile(dst, GFPARM_RMODE_REREAD | GFPARM_RMODE_CREAT);

    // Write data compression type
    char val[32];
    sprintf(val, "%d", m_dataCompressionType);
    GfParmSetStr(readParam, PRM_DATA_COMPRESSION, GFMNU_ATTR_SELECTED, val);

    // Save custom compression to xml file
    char buf[32];
    sprintf(buf, "%d", m_customCompressionLevel);
    GfParmSetStr(readParam, PRM_CUSTOM_COMPRESSION, GFMNU_ATTR_TEXT, buf);

    // Write queued changes
    GfParmWriteFile(nullptr, readParam, COMPRESSION_SCREEN_NAME);
}

/// @brief Saves the settings so the mediator (or future instances) can access them
static void SaveSettings()
{
    int compressionLevel;
    switch (m_customCompressionLevel)
    {
        case 0:  // none
        {
            compressionLevel = 1;
            break;
        }
        case 1:  // Minimum
        {
            compressionLevel = 3;
            break;
        }
        case 2:  // Medium
        {
            compressionLevel = 5;
            break;
        }
        case 3:  // Maximum
        {
            compressionLevel = 9;
            break;
        }
        default:  // Custom
        {
            compressionLevel = m_customCompressionLevel;
            break;
        }
    }

    SMediator* mediator = SMediator::GetInstance();
    mediator->SetCompressionLevel(compressionLevel);

    SaveSettingsToFile();
}

/// @brief Takes actions that need to be run on activation of the data selection screen
static void OnActivate(void* /* dummy */)
{
    LoadSettings();
}

/// @brief Switches back to the data selection menu
static void SwitchToDataSelectionMenu(void* /* dummy */)
{
    // go back to the researcher screen
    GfuiScreenActivate(s_prevHandle);
}

/// @brief Saves the settings and then switched back to the data selection menu
static void SaveAndGoBack(void* /* dummy */)
{
    SaveSettings();

    SwitchToDataSelectionMenu(nullptr);
}

/// @brief        Sets the type of data compression
/// @param p_info Information about the radio button list
static void SelectCompression(tRadioButtonInfo* p_info)
{
    m_dataCompressionType = static_cast<DataCompressionType>(p_info->Selected);
}

/// @brief Handle input in the custom compression type textbox
static void SetCompressionLevel(void*)
{
    char* val = GfuiEditboxGetString(s_scrHandle, m_customCompressionLevelControl);
    char* endptr;
    m_customCompressionLevel = (int)strtol(val, &endptr, 0);
    if (*endptr != '\0')
        std::cerr << "Could not convert " << val << " to long and leftover string is: " << endptr << std::endl;

    // the compression level needs to be uneven
    if (m_customCompressionLevel % 2 == 0)
    {
        m_customCompressionLevel++;
    }

    if (m_customCompressionLevel > MAX_COMPRESSION_LEVEL)
    {
        m_customCompressionLevel = MAX_COMPRESSION_LEVEL;
    }
    else if (m_customCompressionLevel < 1)
    {
        m_customCompressionLevel = 1;
    }

    char buf[32];
    sprintf(buf, "%d", m_customCompressionLevel);
    GfuiEditboxSetString(s_scrHandle, m_customCompressionLevelControl, buf);
}

/// @brief            Initializes the data compression menu
/// @param p_prevMenu A handle to the previous menu
/// @returns          A handle to the data compression menu
void* DataCompressionMenuInit(void* p_prevMenu)
{
    // screen already created
    if (s_scrHandle) return s_scrHandle;

    s_scrHandle = GfuiScreenCreate((float*)nullptr, nullptr, OnActivate, nullptr,
                                   (tfuiCallback) nullptr, 1);
    s_prevHandle = p_prevMenu;

    void* param = GfuiMenuLoad("DataCompressionMenu.xml");
    GfuiMenuCreateStaticControls(s_scrHandle, param);

    GfuiMenuCreateButtonControl(s_scrHandle, param, "CancelButton",
                                s_scrHandle, SwitchToDataSelectionMenu);
    m_applyButtonComp = GfuiMenuCreateButtonControl(s_scrHandle, param, "ApplyButton",
                                                    s_scrHandle, SaveAndGoBack);
    m_compressionButtonList = GfuiMenuCreateRadioButtonListControl(s_scrHandle, param, PRM_DATA_COMPRESSION, nullptr, SelectCompression);

    // Textbox controls
    m_customCompressionLevelControl = GfuiMenuCreateEditControl(s_scrHandle, param, PRM_CUSTOM_COMPRESSION, nullptr, nullptr, SetCompressionLevel);

    GfParmReleaseHandle(param);

    GfuiAddKey(s_scrHandle, GFUIK_RETURN, "Apply", nullptr, SaveAndGoBack, nullptr);
    GfuiAddKey(s_scrHandle, GFUIK_ESCAPE, "Cancel", s_prevHandle, SwitchToDataSelectionMenu, nullptr);
    GfuiMenuDefaultKeysAdd(s_scrHandle);

    return s_scrHandle;
}

/// @brief Activates the data compression menu
void DataCompressionMenuRun(void*)
{
    GfuiScreenActivate(s_scrHandle);
}

/// @brief Makes sure these settings are still set in the SDAConfig, even if this menu is never opened and exited via apply,
/// as otherwise there is no guarantee on what the settings are.
void ConfigureDataCompressionSettings()
{
    LoadSettings();
    SaveSettings();
}