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
/// @param p_raceEndType enum that decided how you got to the save screen
void* ConfirmationMenuInit(void* p_prevMenu, RaceEndType p_raceEndType)
{
    if (s_menuHandle)
    {
        GfuiScreenRelease(s_menuHandle);
    }

    s_menuHandle = GfuiScreenCreate();

    void* param = GfuiMenuLoad("confirmationmenu.xml");
    GfuiMenuCreateStaticControls(s_menuHandle, param);
    switch (p_raceEndType)  ////add different button functionality based on the RaceEndType
    {
        case RACE_EXIT:
        {
            GfuiMenuCreateButtonControl(s_menuHandle, param, PRM_IMSURE_BUTTON, nullptr, OnAcceptExit);
            break;
        }
        case RACE_RESTART:
        {
            GfuiMenuCreateButtonControl(s_menuHandle, param, PRM_IMSURE_BUTTON, nullptr, OnAcceptRestart);
            break;
        }
        case RACE_ABORT:
        {
            GfuiMenuCreateButtonControl(s_menuHandle, param, PRM_IMSURE_BUTTON, nullptr, OnAcceptAbort);
            break;
        }
        case RACE_FINISHED:
        {
            GfuiMenuCreateButtonControl(s_menuHandle, param, PRM_IMSURE_BUTTON, nullptr, OnAcceptAbort);
            break;
        }
        default:
        {
            // throws an error, invalid option
            throw std::runtime_error("incorrect 'p_raceEndType', have you defined the new option in ConfigEnum.h?");
        }
    }
    // add button functionality
    GfuiMenuCreateButtonControl(s_menuHandle, param, PRM_DONTDELTE_BUTTON, p_prevMenu, GfuiScreenActivate);

    GfParmReleaseHandle(param);

    GfuiMenuDefaultKeysAdd(s_menuHandle);
    // add keyboard key functionality
    GfuiAddKey(s_menuHandle, GFUIK_ESCAPE, "Wait, don't delete the data", p_prevMenu, GfuiScreenActivate, nullptr);

    return s_menuHandle;
}
