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
/// @param p_raceEndType enum that decided how you got to the save screen
void* SaveMenuInit(void* p_prevMenu, RaceEndType p_raceEndType)
{
    if (s_menuHandle)
    {
        GfuiScreenRelease(s_menuHandle);
    }

    s_menuHandle = GfuiScreenCreate();

    void* param = GfuiMenuLoad("savemenu.xml");
    GfuiMenuCreateStaticControls(s_menuHandle, param);
    // add button functionality
    GfuiMenuCreateButtonControl(s_menuHandle, param, PRM_DONTSAVE_BUTTON, ConfirmationMenuInit(s_menuHandle, p_raceEndType), GfuiScreenActivate);
    switch (p_raceEndType)  // add different button functionality based on the RaceEndType
    {
        case RACE_EXIT:
        {
            GfuiMenuCreateButtonControl(s_menuHandle, param, PRM_YESSAVE_BUTTON, nullptr, OnAcceptExit);
            break;
        }
        case RACE_RESTART:
        {
            GfuiMenuCreateButtonControl(s_menuHandle, param, PRM_YESSAVE_BUTTON, nullptr, OnAcceptRestart);
            break;
        }
        case RACE_ABORT:
        {
            GfuiMenuCreateButtonControl(s_menuHandle, param, PRM_YESSAVE_BUTTON, nullptr, OnAcceptAbort);
            break;
        }
        case RACE_FINISHED:
        {
            GfuiMenuCreateButtonControl(s_menuHandle, param, PRM_YESSAVE_BUTTON, nullptr, OnAcceptAbort);
            break;
        }
        default:
        {
            // throws an error, invalid option
            throw std::runtime_error("incorrect 'p_raceEndType', have you defined the new option in ConfigEnum.h?");
        }
    }  //*/
    GfParmReleaseHandle(param);

    GfuiMenuDefaultKeysAdd(s_menuHandle);
    // add keyboard key functionality
    GfuiAddKey(s_menuHandle, GFUIK_ESCAPE, "Wait, changed my mind", p_prevMenu, GfuiScreenActivate, nullptr);

    return s_menuHandle;
}