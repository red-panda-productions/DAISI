/**************************************************************************

    file        : racestartstop.cpp
    copyright   : (C) 2011 by Jean-Philippe Meuret
    email       : pouillot@users.sourceforge.net
    version     : $Id: racestopmenu.cpp 6922 2020-04-15 15:29:02Z iobyte $
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include <portability.h>

#include <tgfclient.h>

#include <isoundengine.h>

#include <controlconfig.h>

#include "legacymenu.h"
#include "exitmenu.h"
#include "SaveMenu.h"
#include "racescreens.h"

#include <graphic.h>
#include <playerpref.h>
#include <robot.h>

#if SDL_FORCEFEEDBACK
#include <forcefeedbackconfig.h>
#endif

extern RmProgressiveTimeModifier rmProgressiveTimeModifier;

extern bool rmPreRacePause;

static void *hscreen = 0;
static int curPlayerIdx = 0;

// Abort race hook ******************************************************
static void
rmAbortRaceHookActivate(void * /* dummy */)
{
    LmRaceEngine().abortRace();
}

static void *pvAbortRaceHookHandle = 0;

static void *
rmAbortRaceHookInit()
{
    if (!pvAbortRaceHookHandle)
        pvAbortRaceHookHandle = GfuiHookCreate(0, rmAbortRaceHookActivate);

    return pvAbortRaceHookHandle;
}

// Skip session hook ***************************************************
static void
rmSkipSessionHookActivate(void * /* dummy */)
{
    LmRaceEngine().skipRaceSession();
}

static void *pvSkipSessionHookHandle = nullptr;

static void *
rmSkipSessionHookInit()
{
    if (!pvSkipSessionHookHandle)
        pvSkipSessionHookHandle = GfuiHookCreate(0, rmSkipSessionHookActivate);

    return pvSkipSessionHookHandle;
}

// Back to race hook ***************************************************
static void
rmBackToRaceHookActivate(void * /* dummy */)
{
    // Temporary hack for the Paused race case, in order
    // the race does not get ended (as is is currently stopped)
    // TODO: Activate the Stop Race menu directly, as for the Help menu (F1),
    //      and no more through changing the race engine state to STOP
    //      But beware of the other hooks ...
    LmRaceEngine().inData()->_reState = RE_STATE_RACE;

    // Back to the race Screen in next display loop.
    LegacyMenu::self().activateGameScreen();

    // SIMULATED DRIVING ASSISTANCE CHANGE: Disable time modifier when unpausing
}

static void *pvBackToRaceHookHandle = nullptr;

void *
RmBackToRaceHookInit()
{
    if (!pvBackToRaceHookHandle)
        pvBackToRaceHookHandle = GfuiHookCreate(0, rmBackToRaceHookActivate);

    return pvBackToRaceHookHandle;
}

// Restart race hook ***************************************************
static void *pvRestartRaceHookHandle = nullptr;

static void
rmRestartRaceHookActivate(void * /* dummy */)
{
    if (pvRestartRaceHookHandle)
        GfuiScreenActivate(SaveMenuInit(pvRestartRaceHookHandle, 1));

    // LmRaceEngine().restartRace();
}

static void *
rmRestartRaceHookInit()
{
    if (!pvRestartRaceHookHandle)
        pvRestartRaceHookHandle = GfuiHookCreate(0, rmRestartRaceHookActivate);

    return pvRestartRaceHookHandle;
}

// SIMULATED DRIVING ASSISTANCE: removed controls settings
#if SDL_FORCEFEEDBACK
// ForceFeedbackConfig hook ********************************************
static void
rmForceFeedbackConfigHookActivate(void * /* dummy */)
{
    void *prHandle;
    char buf[100];

    sprintf(buf, "%s%s", GfLocalDir(), HM_PREF_FILE);
    prHandle = GfParmReadFile(buf, GFPARM_RMODE_REREAD);

    snprintf(buf, sizeof(buf), "%s/%s/%d", HM_SECT_PREF, HM_LIST_DRV, curPlayerIdx);

    std::string carName = "";

    // Find human cars
    tRmInfo *pCurrReInfo = LmRaceEngine().inData();
    for (int i = 0; i < pCurrReInfo->s->_ncars; i++)
    {
        if (pCurrReInfo->s->cars[i]->_driverType == RM_DRV_HUMAN)
        {
            carName.append(pCurrReInfo->s->cars[i]->_carName);
        }
    }

    GfuiScreenActivate(ForceFeedbackMenuInit(hscreen, prHandle, curPlayerIdx, carName));
}

static void *pvForceFeedbackConfigHookHandle = nullptr;

static void *
rmForceFeedbackConfigHookInit()
{
    if (!pvForceFeedbackConfigHookHandle)
        pvForceFeedbackConfigHookHandle = GfuiHookCreate(0, rmForceFeedbackConfigHookActivate);

    return pvForceFeedbackConfigHookHandle;
}
#endif

// Quit race hook ******************************************************
static void *rmStopScrHandle = nullptr;

static void
rmQuitHookActivate(void * /* dummy */)
{
    if (rmStopScrHandle)
        GfuiScreenActivate(ExitMenuInit(rmStopScrHandle, true, 2));
}

static void *pvQuitHookHandle = nullptr;

static void *
rmQuitHookInit()
{
    if (!pvQuitHookHandle)
        pvQuitHookHandle = GfuiHookCreate(nullptr, rmQuitHookActivate);

    return pvQuitHookHandle;
}

// 2, 3, 4 or 5 buttons "Stop race" menu *******************************

static void *QuitHdle[6] = {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr};

// Descriptor for 1 button.
typedef struct
{
    const char *Role;  // Button Role.
    void *Screen;      // Screen to activate if clicked.

} tButtonDesc;

// Generic function for creating and activating the menu.
static void *
rmStopRaceMenu(const tButtonDesc p_aButtons[], int p_nButtons, int p_nCancelIndex)
{
    // Create Screen, load menu XML descriptor and create static controls.
    hscreen = GfuiScreenCreate(NULL, NULL, NULL, NULL, NULL, 1);

    void *hmenu = GfuiMenuLoad("stopracemenu.xml");

    GfuiMenuCreateStaticControls(hscreen, hmenu);

    // Create buttons from menu properties and button template.
    const int xpos = (int)GfuiMenuGetNumProperty(hmenu, "xButton", 270);
    const int dy = (int)GfuiMenuGetNumProperty(hmenu, "buttonShift", 30);
    int ypos = (int)GfuiMenuGetNumProperty(hmenu, "yTopButton", 380);
    char pszPropName[64];
    const char *pszCancelTip = "";
    for (int nButInd = 0; nButInd < p_nButtons; nButInd++)
    {
        // Get text and tip from button Role and menu properties.
        sprintf(pszPropName, "%s.text", p_aButtons[nButInd].Role);
        const char *pszText = GfuiMenuGetStrProperty(hmenu, pszPropName, "");
        sprintf(pszPropName, "%s.tip", p_aButtons[nButInd].Role);
        const char *pszTip = GfuiMenuGetStrProperty(hmenu, pszPropName, "");
        if (nButInd == p_nCancelIndex)
            pszCancelTip = pszTip;

        // Create the button from the template.
        GfuiMenuCreateTextButtonControl(hscreen, hmenu, "button",
                                        p_aButtons[nButInd].Screen, GfuiScreenActivate, nullptr, nullptr, nullptr,
                                        true,  // From template.
                                        pszText, pszTip, xpos, ypos);

        // Next button if not last.
        ypos -= dy;
    }

    // Register keyboard shortcuts.
    GfuiMenuDefaultKeysAdd(hscreen);
    GfuiAddKey(hscreen, GFUIK_ESCAPE, pszCancelTip,
               p_aButtons[p_nCancelIndex].Screen, GfuiScreenActivate, NULL);

    // Close menu XML descriptor.
    GfParmReleaseHandle(hmenu);

    // Activate the created Screen.
    GfuiScreenActivate(hscreen);

    return hscreen;
}

// Simpler front-end function for creating and activating the menu.
static void *
rmStopRaceMenu(const char *p_buttonRole1, void *p_screen1,
               const char *p_buttonRole2, void *p_screen2,
               const char *p_buttonRole3 = nullptr, void *p_screen3 = nullptr,
               const char *buttonRole4 = nullptr, void *screen4 = nullptr,
               const char *buttonRole5 = nullptr, void *screen5 = nullptr,
               const char *p_buttonRole6 = nullptr, void *p_screen6 = nullptr)
{
    const tButtonDesc aButtons[6] =
        {
            {p_buttonRole1, p_screen1},
            {p_buttonRole2, p_screen2},
            {p_buttonRole3, p_screen3},
            {buttonRole4, screen4},
            {buttonRole5, screen5},
            {p_buttonRole6, p_screen6},
        };

    int nButtons = 2;
    if (p_buttonRole3 && p_screen3)
    {
        nButtons++;
        if (buttonRole4 && screen4)
        {
            nButtons++;
            if (buttonRole5 && screen5)
            {
                nButtons++;
                if (p_buttonRole6 && p_screen6)
                {
                    nButtons++;
                }
            }
        }
    }

    if (QuitHdle[nButtons - 1])
        GfuiScreenRelease(QuitHdle[nButtons - 1]);

    QuitHdle[nButtons - 1] = rmStopRaceMenu(aButtons, nButtons, nButtons - 1);

    return QuitHdle[nButtons - 1];
}

// SIMULATED DRIVING ASSISTANCE: Assisted driver is also a human
void RmStopRaceMenu()
{
    void *params = LmRaceEngine().outData()->params;
    const char *pszRaceName = LmRaceEngine().outData()->_reRaceName;

    const char *buttonRole[6];
    void *screen[6];
    int i;

#if 1
    int j;
    void *grHandle;
    void *hdHandle;
    void *ahdHandle;
    char buf[100];
    const char *cur_name;
    const char *human_test_name;
    const char *assisted_test_name;

    sprintf(buf, "%s%s", GfLocalDir(), GR_PARAM_FILE);
    grHandle = GfParmReadFile(buf, GFPARM_RMODE_REREAD);

    sprintf(buf, "%s%s", GfLocalDir(), HM_DRV_FILE);
    hdHandle = GfParmReadFile(buf, GFPARM_RMODE_REREAD);

    sprintf(buf, "%s%s", GfLocalDir(), HM_ADRV_FILE);
    ahdHandle = GfParmReadFile(buf, GFPARM_RMODE_REREAD);
#endif

    // Mute sound.
    if (LegacyMenu::self().soundEngine())
        LegacyMenu::self().soundEngine()->mute();

    for (i = 0; i < 6; i++)
    {
        buttonRole[i] = "";
        screen[i] = nullptr;
    }

    // Build list of options
    i = 0;
    buttonRole[i] = "resume";
    screen[i++] = RmBackToRaceHookInit();

    if (strcmp(GfParmGetStr(params, pszRaceName, RM_ATTR_MUST_COMPLETE, RM_VAL_YES), RM_VAL_YES))
    {
        buttonRole[i] = "skip";
        screen[i++] = rmSkipSessionHookInit();
    }

    if (strcmp(GfParmGetStr(params, pszRaceName, RM_ATTR_ALLOW_RESTART, RM_VAL_NO), RM_VAL_NO))
    {
        buttonRole[i] = "restart";
        screen[i++] = rmRestartRaceHookInit();
    }

    buttonRole[i] = "abort";
    screen[i++] = rmAbortRaceHookInit();
#if 1
    // get current driver
    j = (int)GfParmGetNum(grHandle, GR_SCT_DISPMODE, GR_ATT_CUR_SCREEN, NULL, 0.0);
    snprintf(buf, sizeof(buf), "%s/%d", GR_SCT_DISPMODE, j);
    cur_name = GfParmGetStr(grHandle, buf, GR_ATT_CUR_DRV, "not found");
    GfLogInfo("Current driver (on active split Screen) is '%s'\n", cur_name);

    // Attempt to find a human driver
    for (j = 0;; j++)
    {
        snprintf(buf, sizeof(buf), "%s/%s/%d", ROB_SECT_ROBOTS, ROB_LIST_INDEX, j + 1);
        human_test_name = GfParmGetStr(hdHandle, buf, ROB_ATTR_NAME, "");
        assisted_test_name = GfParmGetStr(ahdHandle, buf, ROB_ATTR_NAME, "");

        if (strlen(human_test_name) == 0 && strlen(assisted_test_name) == 0) break;

        if (strcmp(cur_name, human_test_name) == 0 || strcmp(cur_name, assisted_test_name) == 0)
        {
            GfLogInfo("Matching human driver found, setting index to %d.\n", j + 1);
            curPlayerIdx = j + 1;

#if SDL_FORCEFEEDBACK
            buttonRole[i] = "forcefeedback";
            screen[i++] = rmForceFeedbackConfigHookInit();
            break;
#endif
        }
    }
#endif

    buttonRole[i] = "quit";
    screen[i++] = rmQuitHookInit();

    // SIMULATED DRIVING ASSISTANCE: removed controls menu's
    rmStopScrHandle = rmStopRaceMenu(buttonRole[0], screen[0],
                                     buttonRole[1], screen[1],
                                     buttonRole[2], screen[2],
                                     buttonRole[3], screen[3],
                                     buttonRole[4], screen[4],
                                     buttonRole[5], screen[5]);
}

void RmStopRaceMenuShutdown()
{
    GfuiHookRelease(pvAbortRaceHookHandle);
    pvAbortRaceHookHandle = nullptr;

    GfuiHookRelease(pvSkipSessionHookHandle);
    pvSkipSessionHookHandle = 0;

    GfuiHookRelease(pvBackToRaceHookHandle);
    pvBackToRaceHookHandle = 0;

    GfuiHookRelease(pvRestartRaceHookHandle);
    pvRestartRaceHookHandle = 0;

    GfuiHookRelease(pvQuitHookHandle);
    pvQuitHookHandle = 0;
}
