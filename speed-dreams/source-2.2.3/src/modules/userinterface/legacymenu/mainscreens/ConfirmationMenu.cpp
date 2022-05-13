#include <tgfclient.h>

#include "legacymenu.h"
#include "ConfigEnums.h"
#include "mediator.h"
#include "ConfirmationMenu.h"
#include "mainmenu.h"

static void* s_menuHandle = nullptr;

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
    if (s_menuHandle)
    {
        GfuiScreenRelease(s_menuHandle);
    }

    s_menuHandle = GfuiScreenCreate();

    void* param = GfuiMenuLoad("confirmationmenu.xml");
    GfuiMenuCreateStaticControls(s_menuHandle, param);
    switch (p_saveWayVersion)  ////add different button functionality based on the RaceEndType
    {
        case EXIT:
        {
            GfuiMenuCreateButtonControl(s_menuHandle, param, "imsure", nullptr, OnAcceptExit);
            break;
        }
        case RESTART:
        {
            GfuiMenuCreateButtonControl(s_menuHandle, param, "imsure", nullptr, OnAcceptRestart);
            break;
        }
        case ABORT:
        {
            GfuiMenuCreateButtonControl(s_menuHandle, param, "imsure", nullptr, OnAcceptAbort);
            break;
        }
        default:
        {
            // throws an error, invalid option
            throw std::runtime_error("incorrect 'p_saveWayVersion', have you defined the new option in ConfigEnum.h?");
        }
    }
    // add button functionality
    GfuiMenuCreateButtonControl(s_menuHandle, param, "waitdontdelete", p_prevMenu, GfuiScreenActivate);

    GfParmReleaseHandle(param);

    GfuiMenuDefaultKeysAdd(s_menuHandle);
    // add keyboard key functionality
    GfuiAddKey(s_menuHandle, GFUIK_ESCAPE, "Wait, don't delete the data", p_prevMenu, GfuiScreenActivate, nullptr);

    return s_menuHandle;
}
