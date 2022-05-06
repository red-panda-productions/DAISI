#include <tgfclient.h>
#include "legacymenu.h"
#include "mainmenu.h"
#include "DeveloperMenu.h"
#include "ConfigEnums.h"
#include "Mediator.h"

// Parameters used in the xml files
#define PRM_SYNC "SynchronizationButtonList"

static void *s_scrHandle = NULL;
static void *s_prevHandle = NULL;

int m_syncButtonList;

SyncType m_sync;

static void LoadSettings()
{
    
}

static void OnActivate(void * /* dummy */)
{
    LoadSettings();
}

static void SwitchToResearcherMenu(void * /* dummy */)
{
    // go back to the main screen
    GfuiScreenActivate(s_prevHandle);
}

static void SaveSettings(void *)
{
    SMediator* mediator = SMediator::GetInstance();
    mediator->SetSyncOption(m_sync == 1);

    SwitchToResearcherMenu(nullptr);
}

static void SelectSync(tRadioButtonInfo* p_info)
{
    m_sync = (SyncType)p_info->Selected;
}

void *DeveloperMenuInit(void *prevMenu)
{
    // screen already created
    if (s_scrHandle) return s_scrHandle;

    s_scrHandle = GfuiScreenCreate((float *)NULL, NULL, OnActivate, NULL,
                                   (tfuiCallback)NULL, 1);
    s_prevHandle = prevMenu;

    void *param = GfuiMenuLoad("DeveloperMenu.xml");
    GfuiMenuCreateStaticControls(s_scrHandle, param);
    // add UI button controls here
    GfuiMenuCreateButtonControl(s_scrHandle, param, "CancelButton",
                                s_scrHandle, SwitchToResearcherMenu);
    GfuiMenuCreateButtonControl(s_scrHandle, param, "ApplyButton",
                                s_scrHandle, SaveSettings);
    m_syncButtonList = GfuiMenuCreateRadioButtonListControl(s_scrHandle, param, PRM_SYNC, nullptr, SelectSync);

    GfParmReleaseHandle(param);
    // add keyboard controls here
    GfuiAddKey(s_scrHandle, GFUIK_RETURN, "Apply", NULL, SaveSettings, NULL);
    GfuiAddKey(s_scrHandle, GFUIK_ESCAPE, "Cancel", s_prevHandle,
               SwitchToResearcherMenu, NULL);

    return s_scrHandle;
}

void DeveloperMenuRun(void*)
{
    GfuiScreenActivate(s_scrHandle);
}