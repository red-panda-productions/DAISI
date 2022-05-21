#include <tgfclient.h>

#include "legacymenu.h"

#include "EndExperimentMenu.h"

#include "ConfigEnums.h"
#include "SaveMenu.h"

static void* s_menuHandle = nullptr;

/// @brief               Loads the EndExperimentMenu.xml after the race results
/// @param p_raceEndType Enum value that describes how you got to EndExperiment screen
void* EndExperimentInit(RaceEndType p_raceEndType)
{
    if (s_menuHandle)
    {
        GfuiScreenRelease(s_menuHandle);
    }

    s_menuHandle = GfuiScreenCreate();

    void* param = GfuiMenuLoad("EndExperimentMenu.xml");
    GfuiMenuCreateStaticControls(s_menuHandle, param);

    GfuiMenuCreateButtonControl(s_menuHandle, param, "ok", SaveMenuInit(s_menuHandle, p_raceEndType), GfuiScreenActivate);
    GfuiAddKey(s_menuHandle, GFUIK_SPACE, "No, back to the game", nullptr, GfuiScreenActivate, nullptr);

    GfParmReleaseHandle(param);

    return s_menuHandle;
}