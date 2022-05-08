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

static void	*pvSkipSessionHookHandle = 0;

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

    // Back to the race screen in next display loop.
    LegacyMenu::self().activateGameScreen();

    // SIMULATED DRIVING ASSISTANCE CHANGE: Disable time modifier when unpausing
}

static void	*pvBackToRaceHookHandle = 0;

void *
RmBackToRaceHookInit()
{
    if (!pvBackToRaceHookHandle)
        pvBackToRaceHookHandle = GfuiHookCreate(0, rmBackToRaceHookActivate);

    return pvBackToRaceHookHandle;
}

// Restart race hook ***************************************************
static void
rmRestartRaceHookActivate(void * /* dummy */)
{
    LmRaceEngine().restartRace();
}

static void	*pvRestartRaceHookHandle = 0;

static void *
rmRestartRaceHookInit()
{
    if (!pvRestartRaceHookHandle)
        pvRestartRaceHookHandle = GfuiHookCreate(0, rmRestartRaceHookActivate);

    return pvRestartRaceHookHandle;
}


// Quit race hook ******************************************************
static void	*rmStopScrHandle = 0;

static void
rmQuitHookActivate(void * /* dummy */)
{
    if (rmStopScrHandle)
        GfuiScreenActivate(ExitMenuInit(rmStopScrHandle));
}

static void	*pvQuitHookHandle = 0;

static void *
rmQuitHookInit()
{
    if (!pvQuitHookHandle)
        pvQuitHookHandle = GfuiHookCreate(0, rmQuitHookActivate);

    return pvQuitHookHandle;
}

// 2, 3, 4 or 5 buttons "Stop race" menu *******************************

static void *QuitHdle[6] = { 0, 0, 0, 0, 0, 0 };

// Descriptor for 1 button.
typedef struct {

    const char* role;  // Button role.
    void       *screen; // Screen to activate if clicked.

} tButtonDesc;

// Generic function for creating and activating the menu.
static void*
rmStopRaceMenu(const tButtonDesc aButtons[], int nButtons, int nCancelIndex)
{
    // Create screen, load menu XML descriptor and create static controls.
    hscreen = GfuiScreenCreate(NULL, NULL, NULL, NULL, NULL, 1);

    void *hmenu = GfuiMenuLoad("stopracemenu.xml");

    GfuiMenuCreateStaticControls(hscreen, hmenu);

    // Create buttons from menu properties and button template.
    const int xpos = (int)GfuiMenuGetNumProperty(hmenu, "xButton", 270);
    const int dy = (int)GfuiMenuGetNumProperty(hmenu, "buttonShift", 30);
    int ypos = (int)GfuiMenuGetNumProperty(hmenu, "yTopButton", 380);
    char pszPropName[64];
    const char* pszCancelTip = "";
    for (int nButInd = 0; nButInd < nButtons; nButInd++)
    {
        // Get text and tip from button role and menu properties.
        sprintf(pszPropName, "%s.text", aButtons[nButInd].role);
        const char* pszText = GfuiMenuGetStrProperty(hmenu, pszPropName, "");
        sprintf(pszPropName, "%s.tip", aButtons[nButInd].role);
        const char* pszTip = GfuiMenuGetStrProperty(hmenu, pszPropName, "");
        if (nButInd == nCancelIndex)
            pszCancelTip = pszTip;

        // Create the button from the template.
        GfuiMenuCreateTextButtonControl(hscreen, hmenu, "button",
                                        aButtons[nButInd].screen, GfuiScreenActivate, 0, 0, 0,
                                        true, // From template.
                                        pszText, pszTip, xpos, ypos);

        // Next button if not last.
        ypos -= dy;
    }

    // Register keyboard shortcuts.
    GfuiMenuDefaultKeysAdd(hscreen);
    GfuiAddKey(hscreen, GFUIK_ESCAPE, pszCancelTip,
               aButtons[nCancelIndex].screen, GfuiScreenActivate, NULL);

    // Close menu XML descriptor.
    GfParmReleaseHandle(hmenu);

    // Activate the created screen.
    GfuiScreenActivate(hscreen);

    return hscreen;
}

// Simpler front-end function for creating and activating the menu.
static void*
rmStopRaceMenu(const char *buttonRole1, void *screen1,
               const char *buttonRole2, void *screen2,
               const char *buttonRole3 = 0, void *screen3 = 0,
               const char *buttonRole4 = 0, void *screen4 = 0,
               const char *buttonRole5 = 0, void *screen5 = 0,
               const char *buttonRole6 = 0, void *screen6 = 0,
               const char *buttonRole7 = 0, void *screen7 = 0)
{
    const tButtonDesc aButtons[7] =
    {
        { buttonRole1, screen1 },
        { buttonRole2, screen2 },
        { buttonRole3, screen3 },
        { buttonRole4, screen4 },
        { buttonRole5, screen5 },
        { buttonRole6, screen6 },
        { buttonRole7, screen7 }
    };

    int nButtons = 2;
    if (buttonRole3 && screen3)
    {
        nButtons++;
        if (buttonRole4 && screen4)
        {
            nButtons++;
            if (buttonRole5 && screen5)
            {
                nButtons++;
                if (buttonRole6 && screen6)
                {
                    nButtons++;
                    if (buttonRole7 && screen7)
                        nButtons++;
                }
            }
        }
    }

    if (QuitHdle[nButtons-1])
        GfuiScreenRelease(QuitHdle[nButtons-1]);

    QuitHdle[nButtons-1] = rmStopRaceMenu(aButtons, nButtons, nButtons-1);

    return QuitHdle[nButtons-1];
}

// SIMULATED DRIVING ASSISTANCE: Assisted driver is also a human
void
RmStopRaceMenu()
{
    void* params = LmRaceEngine().outData()->params;
    const char* pszRaceName = LmRaceEngine().outData()->_reRaceName;

    const char *buttonRole[7];
    void *screen[7];
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

    for(i=0; i < 7; i++) {
        buttonRole[i] = "";
        screen[i] = NULL;
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

    buttonRole[i] = "quit";
    screen[i++] = rmQuitHookInit();

    // SIMULATED DRIVING ASSISTANCE: removed controls and forcefeedback menu's
    rmStopScrHandle = rmStopRaceMenu(buttonRole[0], screen[0],
               buttonRole[1], screen[1],
               buttonRole[2], screen[2],
               buttonRole[3], screen[3],
               0, 0,
               0, 0,
               buttonRole[6], screen[6]);
}

void
RmStopRaceMenuShutdown()
{
    GfuiHookRelease(pvAbortRaceHookHandle);
    pvAbortRaceHookHandle = 0;

    GfuiHookRelease(pvSkipSessionHookHandle);
    pvSkipSessionHookHandle = 0;

    GfuiHookRelease(pvBackToRaceHookHandle);
    pvBackToRaceHookHandle = 0;

    GfuiHookRelease(pvRestartRaceHookHandle);
    pvRestartRaceHookHandle = 0;

    GfuiHookRelease(pvQuitHookHandle);
    pvQuitHookHandle = 0;
}
