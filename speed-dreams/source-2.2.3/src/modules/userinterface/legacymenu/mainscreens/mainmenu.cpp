/***************************************************************************

    file                 : mainmenu.cpp
    created              : Sat Mar 18 23:42:38 CET 2000
    copyright            : (C) 2000 by Eric Espie
    email                : torcs@free.fr
    version              : $Id: mainmenu.cpp 5158 2013-02-17 17:06:28Z wdbee $

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
***************************************************************************/

#include <tgfclient.h>

#include <racescreens.h>
// SIMULATED DRIVING ASSISTANCE: REMOVED PLAYER CONFIG, ADDED CONTROLCONGFIG
#include <controlconfig.h>

#include "mainmenu.h"
#include "exitmenu.h"
#include "optionsmenu.h"
#include "creditsmenu.h"
#include "DataSelectionMenu.h"

// SIMULATED DRIVING ASSISTANCE: RENAMED MenuHandle
static void *MainMenuHandle = 0;


static void
onControlMenuActivate(void* /* dummy */)
{
    /* Here, we need to call OptionOptionInit each time the firing button
       is pressed, and not only once at the Main menu initialization,
       because the previous menu has to be saved (ESC, Back) and because it can be this menu,
       as well as the Raceman menu */
       // SIMULATED DRIVING ASSISTANCE: SKIP PLAYER MENU
    GfuiScreenActivate(ControlMenuInit(MainMenuHandle, 1));
}

static void
onRaceSelectMenuActivate(void * /* dummy */)
{
    GfuiScreenActivate(RmRaceSelectInit(MainMenuHandle));
}

//static void
//onRaceWESelectMenuActivate(void * /* dummy */)
/*{
    GfuiScreenActivate(RmRaceWESelectInit(MainMenuHandle));
}*/

static void
onOptionsMenuActivate(void * /* dummy */)
{
    GfuiScreenActivate(OptionsMenuInit(MainMenuHandle));
}

// SIMULATED DRIVING ASSISTANCE CHANGE: added GoBack function
/// @brief Activates the dataSelectionMenu screen
static void GoBack(void * /* dummy */)
{
    GfuiScreenActivate(DataSelectionMenuInit(MainMenuHandle));
}

static void
onCreditsMenuActivate(void * /* dummy */)
{
    CreditsMenuActivate(MainMenuHandle);
}

static void
onExitMenuActivate(void * /*dummy*/)
{
    GfuiScreenActivate(ExitMenuInit(MainMenuHandle));
}

static void
onMainMenuActivate(void * /* dummy */)
{
}

/*
 * Function
 *	MainMenuInit
 *
 * Description
 *	init the main menu
 *
 * Parameters
 *	none
 *
 * Return
 *	0 ok -1 nok
 *
 * Remarks
 *
 */

void*
MainMenuInit(bool SupportsHumanDrivers)
{
    // Initialize only once.
    if (MainMenuHandle)
        return MainMenuHandle;

    MainMenuHandle = GfuiScreenCreate((float*)NULL,
        NULL, onMainMenuActivate,
        NULL, (tfuiCallback)NULL,
        1);

    void* menuDescHdle = GfuiMenuLoad("mainmenu.xml");

    GfuiMenuCreateStaticControls(MainMenuHandle, menuDescHdle);

    //Add buttons and create based on xml
    // SIMULATED DRIVING ASSISTANCE: FROM RACE TO START
    GfuiMenuCreateButtonControl(MainMenuHandle, menuDescHdle, "start", NULL, onRaceSelectMenuActivate);
    //GfuiMenuCreateButtonControl(MainMenuHandle, menuDescHdle, "weekend", NULL, onRaceWESelectMenuActivate);
    if (SupportsHumanDrivers)
        GfuiMenuCreateButtonControl(MainMenuHandle, menuDescHdle, "configure", NULL, onControlMenuActivate);
    GfuiMenuCreateButtonControl(MainMenuHandle, menuDescHdle, "options", NULL, onOptionsMenuActivate);
    GfuiMenuCreateButtonControl(MainMenuHandle, menuDescHdle, "credits", NULL, onCreditsMenuActivate);

    // SIMULATED DRIVING ASSISTANCE CHANGE: added back button
    GfuiMenuCreateButtonControl(MainMenuHandle, menuDescHdle, "back", nullptr, GoBack);

    GfuiMenuCreateButtonControl(MainMenuHandle, menuDescHdle, "quit", NULL, onExitMenuActivate);

    GfParmReleaseHandle(menuDescHdle);

    GfuiMenuDefaultKeysAdd(MainMenuHandle);
    GfuiAddKey(MainMenuHandle, GFUIK_ESCAPE, "Quit the game", NULL, onExitMenuActivate, NULL);

    return MainMenuHandle;
}

/*
 * Function
 *	MainMenuRun
 *
 * Description
 *	Activate the main menu
 *
 * Parameters
 *	none
 *
 * Return
 *	0 ok -1 nok
 *
 * Remarks
 *
 */
int
MainMenuRun(void)
{
    GfuiScreenActivate(MainMenuHandle);

    return 0;
}
