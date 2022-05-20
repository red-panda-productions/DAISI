#include <tgfclient.h>

#include "legacymenu.h"

#include "Mediator.h"
#include "mainmenu.h"

static void* BeginExperimentMenuHandle = nullptr;

/// @brief     This function activates the loading screen
static void OnAcceptExit(void* /* dummy */)
{
    //LegacyMenu legacyMenu = legacyMenu.self();
    //legacyMenu.load()
    //legacyMenu.
    //legacyMenu.activateLoadingScreen();
    LegacyMenu::self().activateLoadingScreen();
}

/// @brief                
/// @param p_menuHandle 
/// @return 
void* BeginExperimentInit(void* p_menuHandle)
{
    if (BeginExperimentMenuHandle)
    {
        GfuiScreenRelease(BeginExperimentMenuHandle);
    }

    BeginExperimentMenuHandle = GfuiScreenCreate();

    void* param = GfuiMenuLoad("BeginExperimentMenu.xml");
    GfuiMenuCreateStaticControls(BeginExperimentMenuHandle, param);

    GfuiMenuCreateButtonControl(BeginExperimentMenuHandle, param, "ok", nullptr, OnAcceptExit);

    GfuiAddKey(BeginExperimentMenuHandle, GFUIK_SPACE, "Go to the experiment", nullptr, OnAcceptExit, nullptr);

    GfParmReleaseHandle(param);

    return BeginExperimentMenuHandle;
}