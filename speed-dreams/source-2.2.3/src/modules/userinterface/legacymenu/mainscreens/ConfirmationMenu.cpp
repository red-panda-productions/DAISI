#include <tgfclient.h>

#include "legacymenu.h"
#include "ConfigEnums.h"
#include "mediator.h"
#include "ConfirmationMenu.h"
#include "mainmenu.h"

static void* MenuHandle = nullptr;

/// @brief tells the mediator to not save experiment data and close SpeedDreams
static void OnAcceptExit(void* /* dummy */)
{
    SMediator::GetInstance()->SetSaveRaceToDatabase(false);
    LmRaceEngine().abortRace();  // Do cleanup to get back correct setup files
    LegacyMenu::self().quit();
}

/// @brief tells the mediator to not save experiment data and restart SpeedDreams
static void OnAcceptRestart(void* /* dummy */)
{
    SMediator::GetInstance()->SetSaveRaceToDatabase(false);
    LmRaceEngine().restartRace();
}

/// @brief tells the mediator to save experiment data and abort the race
static void OnAcceptAbort(void* /* dummy */)
{
    SMediator::GetInstance()->SetSaveRaceToDatabase(false);
    LmRaceEngine().abortRace();
}

/// @brief                  create a confirmation screen
/// @param p_prevMenu       the previous menu from where it came
/// @param P_saveWayVersion enum that decided how you got to the save screen
void* ConfirmationMenuInit(void* p_prevMenu, RaceEndType p_saveWayVersion)
{
    if (MenuHandle)
    {
        GfuiScreenRelease(MenuHandle);
    }

    MenuHandle = GfuiScreenCreate();

    void* param = GfuiMenuLoad("confirmationmenu.xml");
    GfuiMenuCreateStaticControls(MenuHandle, param);
    switch (p_saveWayVersion)  ////add different button functionality based on the RaceEndType
    {
        case EXIT:
        {
            GfuiMenuCreateButtonControl(MenuHandle, param, "imsure", nullptr, OnAcceptExit);
            break;
        }
        case RESTART:
        {
            GfuiMenuCreateButtonControl(MenuHandle, param, "imsure", nullptr, OnAcceptRestart);
            break;
        }
        case ABORT:
        {
            GfuiMenuCreateButtonControl(MenuHandle, param, "imsure", nullptr, OnAcceptAbort);
            break;
        }
        default:
        {
            // throws an error, invalid option
            throw std::runtime_error("incorrect 'p_saveWayVersion', have you defined the new option in ConfigEnum.h?");
        }
    }
    // add button functionality
    GfuiMenuCreateButtonControl(MenuHandle, param, "waitdontdelete", p_prevMenu, GfuiScreenActivate);

    GfParmReleaseHandle(param);

    GfuiMenuDefaultKeysAdd(MenuHandle);
    // add keyboard key functionality
    GfuiAddKey(MenuHandle, GFUIK_ESCAPE, "Wait, don't delete the data", p_prevMenu, GfuiScreenActivate, nullptr);

    return MenuHandle;
}
