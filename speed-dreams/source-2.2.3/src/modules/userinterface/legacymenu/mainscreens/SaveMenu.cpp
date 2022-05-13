#include <tgfclient.h>

#include "legacymenu.h"
#include "ConfigEnums.h"
#include "Mediator.h"
#include "ConfirmationMenu.h"
#include "SaveMenu.h"
#include "mainmenu.h"

static void* MenuHandle = nullptr;

/// @brief tells the mediator to save experiment data and close SpeedDreams
static void OnAcceptExit(void* /* dummy */)
{
    SMediator::GetInstance()->SetSaveRaceToDatabase(true);
    LmRaceEngine().abortRace();  // Do cleanup to get back correct setup files
    LegacyMenu::self().quit();
}

/// @brief tells the mediator to save experiment data and restart the race
static void OnAcceptRestart(void* /* dummy */)
{
    SMediator::GetInstance()->SetSaveRaceToDatabase(true);
    LmRaceEngine().restartRace();
}

/// @brief tells the mediator to save experiment data and abort the race
static void OnAcceptAbort(void* /* dummy */)
{
    SMediator::GetInstance()->SetSaveRaceToDatabase(true);
    LmRaceEngine().abortRace();
}

/// @brief                  create a save data screen
/// @param p_prevMenu       the previous menu from where it came
/// @param P_saveWayVersion enum that decided how you got to the save screen
void* SaveMenuInit(void* p_prevMenu, RaceEndType p_saveWayVersion)
{
    if (MenuHandle)
    {
        GfuiScreenRelease(MenuHandle);
    }

    MenuHandle = GfuiScreenCreate();

    void* param = GfuiMenuLoad("savemenu.xml");
    GfuiMenuCreateStaticControls(MenuHandle, param);
    // add button functionality
    GfuiMenuCreateButtonControl(MenuHandle, param, "dontsave", ConfirmationMenuInit(MenuHandle, p_saveWayVersion), GfuiScreenActivate);
    switch (p_saveWayVersion)  // add different button functionality based on the RaceEndType
    {
        case EXIT:
        {
            GfuiMenuCreateButtonControl(MenuHandle, param, "yessave", nullptr, OnAcceptExit);
            break;
        }
        case RESTART:
        {
            GfuiMenuCreateButtonControl(MenuHandle, param, "yessave", nullptr, OnAcceptRestart);
            break;
        }
        case ABORT:
        {
            GfuiMenuCreateButtonControl(MenuHandle, param, "yessave", nullptr, OnAcceptAbort);
            break;
        }
        default:
        {
            // throws an error, invalid option
            throw std::runtime_error("incorrect 'p_saveWayVersion', have you defined the new option in ConfigEnum.h?");
        }
    }
    GfParmReleaseHandle(param);

    GfuiMenuDefaultKeysAdd(MenuHandle);
    // add keyboard key functionality
    GfuiAddKey(MenuHandle, GFUIK_ESCAPE, "Wait, changed my mind", p_prevMenu, GfuiScreenActivate, nullptr);

    return MenuHandle;
}