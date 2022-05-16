#include <tgfclient.h>

#include "legacymenu.h"

#include "Mediator.h"
#include "EndExperimentMenu.h"
#include "mainmenu.h"

static void* s_menuHandle = NULL;

static void OnAcceptExit(void* /* dummy */)
{
    LmRaceEngine().abortRace();  // Do cleanup to get back correct setup files
    LegacyMenu::self().quit();
}


void* EndExperimentInit(void* p_menuHandle, RaceEndType p_saveWayVersion)
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
            GfuiMenuCreateButtonControl(s_menuHandle, param, "ok", NULL, OnAcceptExit);
            break;
        }
        default:
        {
            throw std::runtime_error("incorrect 'p_experimentWayVersion', have you defined the new option in endexperiment.h?");
        }
    }
    GfuiAddKey(s_menuHandle, GFUIK_SPACE, "No, back to the game", NULL, OnAcceptExit, NULL);

    GfParmReleaseHandle(param);

    return s_menuHandle;
}