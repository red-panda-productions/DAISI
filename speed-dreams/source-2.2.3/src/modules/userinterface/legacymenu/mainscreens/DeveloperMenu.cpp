#include <tgfclient.h>
#include "legacymenu.h"
#include "mainmenu.h"
#include "DeveloperMenu.h"

static void *s_scrHandle = NULL;
static void *s_prevHandle = NULL;

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
    SwitchToResearcherMenu(nullptr);
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