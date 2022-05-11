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
#include "savemenu.h"
#include "mainmenu.h"

static void
onAcceptExit(void * /* dummy */)
{
    LmRaceEngine().abortRace();  // Do cleanup to get back correct setup files
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
static void *MenuHandle = nullptr;

void *ExitMenuInit(void *p_prevMenu, bool p_raceExit, int p_saveWayVersion)
{
    if (MenuHandle)
    {
        GfuiScreenRelease(MenuHandle);
    }

    MenuHandle = GfuiScreenCreate();

    void *param = GfuiMenuLoad("exitmenu.xml");

    GfuiMenuCreateStaticControls(MenuHandle, param);
    if (p_raceExit)
    {
        GfuiMenuCreateButtonControl(MenuHandle, param, "yesquit", SaveMenuInit(MenuHandle, p_saveWayVersion), GfuiScreenActivate);
        GfuiMenuCreateButtonControl(MenuHandle, param, "nobacktogame", p_prevMenu, GfuiScreenActivate);
    }
    else
    {
        GfuiMenuCreateButtonControl(MenuHandle, param, "yesquit", nullptr, onAcceptExit);
        GfuiMenuCreateButtonControl(MenuHandle, param, "nobacktogame", p_prevMenu, GfuiScreenActivate);
    }

    GfParmReleaseHandle(param);

    GfuiMenuDefaultKeysAdd(MenuHandle);
    GfuiAddKey(MenuHandle, GFUIK_ESCAPE, "No, back to the game", p_prevMenu, GfuiScreenActivate, nullptr);

    return MenuHandle;
}
