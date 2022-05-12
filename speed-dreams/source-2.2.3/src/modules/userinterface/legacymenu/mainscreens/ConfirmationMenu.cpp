#include <tgfclient.h>

#include "legacymenu.h"
#include "ConfigEnums.h"
#include "mediator.h"
#include "ConfirmationMenu.h"
#include "mainmenu.h"

static void *MenuHandle = nullptr;

static void
onAcceptExit(void * /* dummy */)
{
    SMediator::GetInstance()->SetSaveRaceToDatabase(false);
    LmRaceEngine().abortRace();  // Do cleanup to get back correct setup files
    LegacyMenu::self().quit();
}

static void
onAcceptRestart(void * /* dummy */)
{
    SMediator::GetInstance()->SetSaveRaceToDatabase(false);
    LmRaceEngine().restartRace();
}

void *ConfirmationMenuInit(void *p_prevMenu, RaceEndType p_saveWayVersion)
{
    if (MenuHandle)
    {
        GfuiScreenRelease(MenuHandle);
    }

    MenuHandle = GfuiScreenCreate();

    void *param = GfuiMenuLoad("confirmationmenu.xml");
    GfuiMenuCreateStaticControls(MenuHandle, param);
    switch (p_saveWayVersion)
    {
        case EXIT:
        {
            GfuiMenuCreateButtonControl(MenuHandle, param, "imsure", nullptr, onAcceptExit);
            break;
        }
        case RESTART:
        {
            GfuiMenuCreateButtonControl(MenuHandle, param, "imsure", nullptr, onAcceptRestart);
            break;
        }
        default:
        {
            throw std::runtime_error("incorrect 'p_saveWayVersion', have you defined the new option in conformationmenu.h?");
        }
    }
    GfuiMenuCreateButtonControl(MenuHandle, param, "imsure", nullptr, onAcceptExit);
    GfuiMenuCreateButtonControl(MenuHandle, param, "waitdontdelete", p_prevMenu, GfuiScreenActivate);

    GfParmReleaseHandle(param);

    GfuiMenuDefaultKeysAdd(MenuHandle);
    GfuiAddKey(MenuHandle, GFUIK_ESCAPE, "Wait, don't delete the data", p_prevMenu, GfuiScreenActivate, nullptr);

    return MenuHandle;
}
