#include <tgfclient.h>

#include "legacymenu.h"

#include "Mediator.h"
#include "EndExperiment.h"
#include "mainmenu.h"

static void* MenuHandle = NULL;

static void
onAcceptExit(void* /* dummy */)
{
    //SMediator::GetInstance()->SetSaveRaceToDatabase(false);
    LmRaceEngine().abortRace();  // Do cleanup to get back correct setup files
    LegacyMenu::self().quit();
}

void* EndExperimentInit(int p_saveWayVersion)
{
    if (MenuHandle)
    {
        GfuiScreenRelease(MenuHandle);
    }

    MenuHandle = GfuiScreenCreate();

    void* param = GfuiMenuLoad("endexperimentscreen.xml");
    GfuiMenuCreateStaticControls(MenuHandle, param);
    GfuiMenuCreateStaticControls(MenuHandle, param);


    switch (p_saveWayVersion)
    {
    case OK:
    {
        GfuiMenuCreateButtonControl(MenuHandle, param, "ok", NULL, onAcceptExit);
        break;
    }
    default:
    {
        throw std::runtime_error("incorrect 'p_experimentWayVersion', have you defined the new option in endexperiment.h?");
    }

    }
    GfuiMenuCreateButtonControl(MenuHandle, param, "ok", NULL, onAcceptExit);

    GfParmReleaseHandle(param);

    return MenuHandle;
}