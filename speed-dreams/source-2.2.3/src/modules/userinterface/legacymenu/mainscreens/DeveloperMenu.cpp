#include <tgfclient.h>
#include "legacymenu.h"
#include "mainmenu.h"
#include "DeveloperMenu.h"
#include "ConfigEnums.h"
#include "Mediator.h"
#include "guimenu.h"

// Parameters used in the xml files
#define PRM_SYNC "SynchronizationButtonList"

static void* s_scrHandle = NULL;
static void* s_prevHandle = NULL;

// Control for synchronization option
int m_syncButtonList;

// Synchronization type
SyncType m_sync;

/// @brief Loads the settings from the config file
static void LoadSettingsFromFile(void* p_param)
{
    m_sync = std::stoi(GfParmGetStr(p_param, PRM_SYNC, GFMNU_ATTR_SELECTED, "1"));
}

/// Makes sure all visuals display the internal values
static void SynchronizeControls()
{
    GfuiRadioButtonListSetSelected(s_scrHandle, m_syncButtonList, m_sync);
}

/// Loads (if possible) the settings; otherwise, the control's default settings will be used
static void LoadSettings()
{
    std::string strPath("config/DeveloperMenu.xml");
    char buf[512];
    sprintf(buf, "%s%s", GfLocalDir(), strPath.c_str());
    if (GfFileExists(buf))
    {
        void* param = GfParmReadFile(buf, GFPARM_RMODE_STD);
        // Initialize settings with the retrieved xml file
        LoadSettingsFromFile(param);
        SynchronizeControls();
    }
}

/// Saves the settings to a file
static void SaveSettingsToFile()
{
    std::string dstStr("config/DeveloperMenu.xml");
    char dst[512];
    sprintf(dst, "%s%s", GfLocalDir(), dstStr.c_str());
    void* readParam = GfParmReadFile(dst, GFPARM_RMODE_REREAD | GFPARM_RMODE_CREAT);

    char val[32];
    sprintf(val, "%d", m_sync);
    GfParmSetStr(readParam, PRM_SYNC, GFMNU_ATTR_SELECTED, val);

    // Write queued changes
    GfParmWriteFile(nullptr, readParam, "DeveloperMenu");
}

/// Saves the settings so the mediator (or future instances) can access them
static void SaveSettings()
{
    SMediator* mediator = SMediator::GetInstance();

    mediator->SetSyncOption(m_sync == 1);

    SaveSettingsToFile();
}

static void OnActivate(void* /* dummy */)
{
    LoadSettings();
}

static void SwitchToResearcherMenu(void* /* dummy */)
{
    // go back to the main screen
    GfuiScreenActivate(s_prevHandle);
}

static void SaveAndGoBack(void*)
{
    SaveSettings();

    SwitchToResearcherMenu(nullptr);
}

static void SelectSync(tRadioButtonInfo* p_info)
{
    m_sync = (SyncType)p_info->Selected;
}

void *DeveloperMenuInit(void* prevMenu)
{
    // screen already created
    if (s_scrHandle) return s_scrHandle;

    s_scrHandle = GfuiScreenCreate((float*)NULL, NULL, OnActivate, NULL,
                                   (tfuiCallback)NULL, 1);
    s_prevHandle = prevMenu;

    void *param = GfuiMenuLoad("DeveloperMenu.xml");
    GfuiMenuCreateStaticControls(s_scrHandle, param);
    // add UI button controls here
    GfuiMenuCreateButtonControl(s_scrHandle, param, "CancelButton",
                                s_scrHandle, SwitchToResearcherMenu);
    GfuiMenuCreateButtonControl(s_scrHandle, param, "ApplyButton",
                                s_scrHandle, SaveAndGoBack);
    m_syncButtonList = GfuiMenuCreateRadioButtonListControl(s_scrHandle, param, PRM_SYNC, nullptr, SelectSync);

    GfParmReleaseHandle(param);
    // add keyboard controls here
    GfuiAddKey(s_scrHandle, GFUIK_RETURN, "Apply", NULL, SaveAndGoBack, NULL);
    GfuiAddKey(s_scrHandle, GFUIK_ESCAPE, "Cancel", s_prevHandle,
               SwitchToResearcherMenu, NULL);

    return s_scrHandle;
}

void DeveloperMenuRun(void *)
{
    GfuiScreenActivate(s_scrHandle);
}