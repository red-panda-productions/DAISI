/*
 * This program has been developed by students from the bachelor Computer Science at
 * Utrecht University within the Software Project course.
 * © Copyright Utrecht University (Department of Information and Computing Sciences)
 */

#include "tgfclient.h"

#include "legacymenu.h"
#include "ConfigEnums.h"
#include "Mediator.h"
#include "racescreens.h"
#include "mainmenu.h"

#define PRM_IM_SURE_BUTTON    "imsure"
#define PRM_DONT_DELTE_BUTTON "waitdontdelete"

static void* s_menuHandle = nullptr;

/// @brief tell the mediator to not save the experiment
static void EndActiveExperiment()
{
    SMediator* m_mediator = SMediator::GetInstance();
    m_mediator->CloseRecorder();
    m_mediator->ShutdownBlackBox();
}

/// @brief tells the mediator to not save experiment data and restart SpeedDreams
static void OnAcceptRestart(void* /* dummy */)
{
    EndActiveExperiment();
    LmRaceEngine().restartRace();
}

/// @brief tells the mediator to save experiment data and abort the race
static void OnAcceptAbort(void* p_prevMenu)
{
    EndActiveExperiment();
    LmRaceEngine().abortRace();
    GfuiScreenActivate(MainMenuInit((p_prevMenu)));
    LmRaceEngine().cleanup();
    LegacyMenu::self().shutdownGraphics(/*bUnloadModule=*/true);
}

/// @brief tells the mediator to save experiment data
static void OnAcceptFinished(void* p_prevMenu)
{
    EndActiveExperiment();
    GfuiScreenActivate(MainMenuInit((p_prevMenu)));
    LmRaceEngine().cleanup();
    LegacyMenu::self().shutdownGraphics(/*bUnloadModule=*/true);
}

/// @brief                  create a confirmation screen
/// @param p_prevMenu       the previous menu from where it came
/// @param p_raceEndType enum that decided how you got to the save screen
void* ConfirmationMenuInit(void* p_prevMenu, RaceEndType p_raceEndType)
{
    s_menuHandle = GfuiScreenCreate();
    void* param = GfuiMenuLoad("ConfirmationMenu.xml");
    GfuiMenuCreateStaticControls(s_menuHandle, param);

    // add button functionality
    GfuiMenuCreateButtonControl(s_menuHandle, param, PRM_DONT_DELTE_BUTTON, p_prevMenu, GfuiScreenReplace);

    switch (p_raceEndType)
    {
        case RACE_RESTART:
        {
            GfuiMenuCreateButtonControl(s_menuHandle, param, PRM_IM_SURE_BUTTON, nullptr, OnAcceptRestart);
            break;
        }
        case RACE_ABORT:
        {
            GfuiMenuCreateButtonControl(s_menuHandle, param, PRM_IM_SURE_BUTTON, nullptr, OnAcceptAbort);
            break;
        }
        case RACE_FINISHED:
        {
            GfuiMenuCreateButtonControl(s_menuHandle, param, PRM_IM_SURE_BUTTON, nullptr, OnAcceptFinished);
            break;
        }
        default:
        {
            // throws an error, invalid option
            throw std::runtime_error("incorrect 'p_raceEndType', have you defined the new option in ConfigEnum.h?");
        }
    }

    GfParmReleaseHandle(param);

    GfuiMenuDefaultKeysAdd(s_menuHandle);
    // add keyboard key functionality
    GfuiAddKey(s_menuHandle, GFUIK_ESCAPE, "Wait, don't delete the data", p_prevMenu, GfuiScreenReplace, nullptr);

    return s_menuHandle;
}
