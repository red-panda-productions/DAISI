#include <tgfclient.h>

#include "legacymenu.h"

#include "EndExperimentMenu.h"

static void* s_menuHandle = nullptr;

/// @brief Abort the simulation/race once the 'ok'/'quit' button has been pressed in the EndExperimentMenu
static void OnAcceptExit(void* /* dummy */)
{
    LmRaceEngine().abortRace();  // Do cleanup to get back correct setup files
    LegacyMenu::self().quit();
}

/// @brief Loads the EndExperimentMenu.xml after the race results
void* EndExperimentInit()
{
    if (s_menuHandle)
    {
        GfuiScreenRelease(s_menuHandle);
    }

    s_menuHandle = GfuiScreenCreate();

    void* param = GfuiMenuLoad("EndExperimentMenu.xml");
    GfuiMenuCreateStaticControls(s_menuHandle, param);

    GfuiMenuCreateButtonControl(s_menuHandle, param, "ok", nullptr, OnAcceptExit);
    GfuiAddKey(s_menuHandle, GFUIK_SPACE, "No, back to the game", nullptr, OnAcceptExit, nullptr);

    GfParmReleaseHandle(param);

    return s_menuHandle;
}