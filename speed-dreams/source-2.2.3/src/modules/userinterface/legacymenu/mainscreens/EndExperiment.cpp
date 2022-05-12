#include <tgfclient.h>

#include "legacymenu.h"

#include "Mediator.h"
#include "EndExperiment.h"
#include "mainmenu.h"

static void* MenuHandle = NULL;

static void
OnAcceptExit(void* /* dummy */)
{
    LmRaceEngine().abortRace();  // Do cleanup to get back correct setup files
    LegacyMenu::self().quit();
}

void* EndExperimentInit(void* p_menuHandle, RaceEndType p_saveWayVersion)
{
    if (MenuHandle)
    {
        GfuiScreenRelease(MenuHandle);
    }

    MenuHandle = GfuiScreenCreate();

    void* param = GfuiMenuLoad("endexperimentscreen.xml");
    GfuiMenuCreateStaticControls(MenuHandle, param);

    switch (p_saveWayVersion)
    {
        case NO_END:        
        case EXIT: 
        case ABORT:
        case RESTART:
        {
            GfuiMenuCreateButtonControl(MenuHandle, param, "ok", NULL, OnAcceptExit);
            break;
        }
        default:
        {
            throw std::runtime_error("incorrect 'p_experimentWayVersion', have you defined the new option in endexperiment.h?");
        }
    }
    GfuiAddKey(MenuHandle, GFUIK_SPACE, "No, back to the game", NULL, OnAcceptExit, NULL);

    GfParmReleaseHandle(param);

    return MenuHandle;
}