#include "tgfclient.h"

#include "legacymenu.h"
#include "ConfigEnums.h"
#include "Mediator.h"
#include "racescreens.h"
#include "mainmenu.h"

#define PRM_YES_SAVE_BUTTON  "yessave"
#define PRM_DONT_SAVE_BUTTON "dontsave"

static void* s_menuHandle = nullptr;

/// @brief tells the mediator to save experiment data and close SpeedDreams
static void OnAcceptExit(void* /* dummy */)
{
    SMediator::GetInstance()->SetSaveRaceToDatabase(true);
    LmRaceEngine().abortRace();
    LegacyMenu::self().quit();
}

/// @brief tells the mediator to save experiment data and restart the race
static void OnAcceptRestart(void* /* dummy */)
{
    SMediator::GetInstance()->SetSaveRaceToDatabase(true);
    LmRaceEngine().restartRace();
}

/// @brief tells the mediator to save experiment data and abort the race
static void OnAcceptAbort(void* p_prevMenu)
{
    SMediator::GetInstance()->SetSaveRaceToDatabase(true);
    LmRaceEngine().abortRace();
    GfuiScreenActivate(MainMenuInit((p_prevMenu)));
    LmRaceEngine().cleanup();
    LegacyMenu::self().shutdownGraphics(/*bUnloadModule=*/true);
}

/// @brief tells the mediator to save experiment data
static void OnAcceptFinished(void* p_prevMenu)
{
    SMediator::GetInstance()->SetSaveRaceToDatabase(true);
    GfuiScreenActivate(MainMenuInit((p_prevMenu)));
    LmRaceEngine().cleanup();
    LegacyMenu::self().shutdownGraphics(/*bUnloadModule=*/true);
}

/// @brief                  create a save data screen
/// @param p_prevMenu       the previous menu from where it came
/// @param p_raceEndType enum that decided how you got to the save screen
void* SaveMenuInit(RaceEndType p_raceEndType)
{
    s_menuHandle = GfuiScreenCreate();

    void* param = GfuiMenuLoad("savemenu.xml");
    GfuiMenuCreateStaticControls(s_menuHandle, param);

    // add button functionality
    GfuiMenuCreateButtonControl(s_menuHandle, param, PRM_DONT_SAVE_BUTTON, ConfirmationMenuInit(s_menuHandle, p_raceEndType), GfuiScreenReplace);
    switch (p_raceEndType)
    {
        case RACE_EXIT:
        {
            GfuiMenuCreateButtonControl(s_menuHandle, param, PRM_YES_SAVE_BUTTON, nullptr, OnAcceptExit);
            break;
        }
        case RACE_RESTART:
        {
            GfuiMenuCreateButtonControl(s_menuHandle, param, PRM_YES_SAVE_BUTTON, nullptr, OnAcceptRestart);
            break;
        }
        case RACE_ABORT:
        {
            GfuiMenuCreateButtonControl(s_menuHandle, param, PRM_YES_SAVE_BUTTON, nullptr, OnAcceptAbort);
            break;
        }
        case RACE_FINISHED:
        {
            GfuiMenuCreateButtonControl(s_menuHandle, param, PRM_YES_SAVE_BUTTON, nullptr, OnAcceptFinished);
            break;
        }
        default:
        {
            // throws an error, invalid option
            throw std::runtime_error("incorrect 'p_raceEndType', have you defined the new option in ConfigEnum.h?");
        }
    }  //*/

    GfuiMenuDefaultKeysAdd(s_menuHandle);
    // add keyboard key functionality
    GfuiAddKey(s_menuHandle, GFUIK_ESCAPE, "I don't want to save the data", ConfirmationMenuInit(s_menuHandle, p_raceEndType), GfuiScreenReplace, nullptr);

    if (p_raceEndType == RACE_RESTART)
    {
        GfuiScreenActivate(s_menuHandle);
    }

    return s_menuHandle;
}