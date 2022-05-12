#include <tgfclient.h>

#include "legacymenu.h"

#include "Mediator.h"
#include "EndExperiment.h"
#include "mainmenu.h"

static void* MenuHandle = NULL;

static void
onAcceptExit(void* /* dummy */)
{
    // SMediator::GetInstance()->SetSaveRaceToDatabase(false);
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
    // GfuiMenuCreateStaticControls(MenuHandle, param);

    switch (p_saveWayVersion)
    {
        case NO_END:
        {
            GfuiMenuCreateButtonControl(MenuHandle, param, "ok", NULL, onAcceptExit);

            break;
        }
        case RESTART:
        {
        }
        case EXIT:
        {
        }
        case ABORT:
        {
        }
        default:
        {
            throw std::runtime_error("incorrect 'p_experimentWayVersion', have you defined the new option in endexperiment.h?");
        }
    }
    // GfuiMenuCreateButtonControl(MenuHandle, param, "ok", NULL, onAcceptExit);
    GfuiAddKey(MenuHandle, GFUIK_SPACE, "No, back to the game", NULL, onAcceptExit, NULL);

    GfParmReleaseHandle(param);

    return MenuHandle;
}