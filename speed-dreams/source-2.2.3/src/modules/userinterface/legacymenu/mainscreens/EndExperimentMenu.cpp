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

/// @brief Loads the EndExperimentMenu.xml according to the given RaceEndType
/// @param p_saveWayVersion represents the RaceEndType
void* EndExperimentInit(RaceEndType p_saveWayVersion)
{
    if (s_menuHandle)
    {
        GfuiScreenRelease(s_menuHandle);
    }

    s_menuHandle = GfuiScreenCreate();

    void* param = GfuiMenuLoad("EndExperimentMenu.xml");
    GfuiMenuCreateStaticControls(s_menuHandle, param);

    switch (p_saveWayVersion)
    {
        case NO_END:
        case EXIT:
        case ABORT:
        case RESTART:
        {
            GfuiMenuCreateButtonControl(s_menuHandle, param, "ok", nullptr, OnAcceptExit);
            break;
        }
        default:
        {
            throw std::runtime_error("incorrect 'p_experimentWayVersion', have you defined the new option in endexperiment.h?");
        }
    }
    GfuiAddKey(s_menuHandle, GFUIK_SPACE, "No, back to the game", nullptr, OnAcceptExit, nullptr);

    GfParmReleaseHandle(param);

    return s_menuHandle;
}