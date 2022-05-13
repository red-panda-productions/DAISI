#include <tgfclient.h>

#include "legacymenu.h"
#include "ConfigEnums.h"
#include "Mediator.h"
#include "ConfirmationMenu.h"
#include "SaveMenu.h"
#include "mainmenu.h"

static void* s_menuHandle = nullptr;

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
    if (s_menuHandle)
    {
        GfuiScreenRelease(s_menuHandle);
    }

    s_menuHandle = GfuiScreenCreate();

    void* param = GfuiMenuLoad("savemenu.xml");
    GfuiMenuCreateStaticControls(s_menuHandle, param);
    // add button functionality
    GfuiMenuCreateButtonControl(s_menuHandle, param, "dontsave", ConfirmationMenuInit(s_menuHandle, p_saveWayVersion), GfuiScreenActivate);
    switch (p_saveWayVersion)  // add different button functionality based on the RaceEndType
    {
        case EXIT:
        {
            GfuiMenuCreateButtonControl(s_menuHandle, param, "yessave", nullptr, OnAcceptExit);
            break;
        }
        case RESTART:
        {
            GfuiMenuCreateButtonControl(s_menuHandle, param, "yessave", nullptr, OnAcceptRestart);
            break;
        }
        case ABORT:
        {
            GfuiMenuCreateButtonControl(s_menuHandle, param, "yessave", nullptr, OnAcceptAbort);
            break;
        }
        default:
        {
            // throws an error, invalid option
            throw std::runtime_error("incorrect 'p_saveWayVersion', have you defined the new option in ConfigEnum.h?");
        }
    }
    GfParmReleaseHandle(param);

    GfuiMenuDefaultKeysAdd(s_menuHandle);
    // add keyboard key functionality
    GfuiAddKey(s_menuHandle, GFUIK_ESCAPE, "Wait, changed my mind", p_prevMenu, GfuiScreenActivate, nullptr);

    return s_menuHandle;
}