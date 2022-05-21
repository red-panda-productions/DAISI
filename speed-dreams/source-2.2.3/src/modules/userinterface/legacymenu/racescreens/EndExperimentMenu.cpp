#include "tgfclient.h"

#include "legacymenu.h"
#include "racescreens.h"

static void* s_menuHandle = nullptr;

/// @brief               Loads the EndExperimentMenu.xml after the race results
/// @param p_raceEndType Enum value that describes how you got to EndExperiment screen
void RmShowEndExperiment(RaceEndType p_raceEndType)
{
    // Create screen, load menu XML descriptor and create static controls.
    s_menuHandle = GfuiScreenCreate();
    void* param = GfuiMenuLoad("EndExperimentMenu.xml");
    GfuiMenuCreateStaticControls(s_menuHandle, param);

    void* prevHandle = SaveMenuInit(p_raceEndType);

    // Add "Continue" button
    GfuiMenuCreateButtonControl(s_menuHandle, param, "ok", prevHandle, GfuiScreenReplace);
    GfuiAddKey(s_menuHandle, GFUIK_SPACE, "Continue", prevHandle, GfuiScreenReplace, nullptr);

    GfuiScreenActivate(s_menuHandle);
}