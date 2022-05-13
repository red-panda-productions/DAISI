/***************************************************************************

    file                 : exitmenu.cpp
    created              : Sat Mar 18 23:42:12 CET 2000
    copyright            : (C) 2000 by Eric Espie
    email                : torcs@free.fr
    version              : $Id: exitmenu.cpp 5154 2013-02-17 10:08:28Z wdbee $

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

#include "legacymenu.h"

#include "exitmenu.h"
#include "mainmenu.h"
#include "ConfigEnums.h"
#include "EndExperimentMenu.h"


static void *MenuHandle = NULL;

static void 
onAcceptExit(void * /* dummy */)
{
	LmRaceEngine().abortRace(); // Do cleanup to get back correct setup files
	LegacyMenu::self().quit();
}

/*
 * Function
 *	ExitMenuInit
 *
 * Description
 *	init the exit menus
 *
 * Parameters
 *	prevMenu : Handle of the menu to activate when cancelling the exit action.
 *
 * Return
 *	The menu handle
 *
 * Remarks
 *	
 */

void* ExitMenuInit(void* prevMenu, bool p_raceExit, RaceEndType p_saveWayVersion)
{
    if (MenuHandle) {
		GfuiScreenRelease(MenuHandle);
    }

    MenuHandle = GfuiScreenCreate();

    void *param = GfuiMenuLoad("exitmenu.xml");

    // Looks where the exit game gets called and change the button functionality based on it.
    if (p_raceExit)
    {
        GfuiMenuCreateButtonControl(MenuHandle, param, "yesquit", EndExperimentInit(MenuHandle, p_saveWayVersion), GfuiScreenActivate);
    }
    else
    {
        GfuiMenuCreateButtonControl(MenuHandle, param, "yesquit", nullptr, onAcceptExit);
    }
    GfuiMenuCreateButtonControl(MenuHandle, param, "nobacktogame", prevMenu, GfuiScreenActivate);

    GfParmReleaseHandle(param);
    
    GfuiMenuDefaultKeysAdd(MenuHandle);
    GfuiAddKey(MenuHandle, GFUIK_ESCAPE, "No, back to the game", prevMenu, GfuiScreenActivate, NULL);

    return MenuHandle;
}
