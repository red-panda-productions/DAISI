/***************************************************************************

    file                 : optionsmenu.cpp
    created              : Mon Apr 24 14:22:53 CEST 2000
    copyright            : (C) 2000, 2001 by Eric Espie
    email                : torcs@free.fr
    version              : $Id: optionsmenu.cpp 4986 2012-10-07 18:32:50Z pouillot $

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

#include <displayconfig.h>
#include <monitorconfig.h>
#include <soundconfig.h>
#include <controlconfig.h>

#include "optionsmenu.h"

 // SIMULATED DRIVING ASSISTANCE: removed player, graph, opengl, advanced graph, simu, ai, hostsettings configs
static void *MenuHandle = NULL;

// SDW hack to get access to Monitor menu, doesn't have a defined position yet
// (Uncomment to select Monitor menu otherwise, the Display menu is used)
//#define MonitorMenu 1

#ifndef MonitorMenu

static void
onDisplayMenuActivate(void * /* dummy */)
{
    GfuiScreenActivate(DisplayMenuInit(MenuHandle));
}

#else

static void
onMonitorMenuActivate(void * /* dummy */)
{
    GfuiScreenActivate(MonitorMenuInit(MenuHandle));
}

#endif


static void
onSoundMenuActivate(void * /* dummy */)
{
    GfuiScreenActivate(SoundMenuInit(MenuHandle));
}

// SIMULATED DRIVING ASSISTANCE: renamed from onPlayerConfigMenuActivate
static void
onControlMenuActivate(void* /* dummy */)
{
    /* Here, we need to call OptionOptionInit each time the firing button
       is pressed, and not only once at the Main menu initialization,
       because the previous menu has to be saved (ESC, Back) and because it can be this menu,
       as well as the Raceman menu */
       // SIMULATED DRIVING ASSISTANCE: SKIP PLAYER MENU
    GfuiScreenActivate(ControlMenuInit(MenuHandle, 1));
}

//SIMULATED DRIVING ASSISTANCE: removed graphics, advanced graphics, opengl, simulation, ai opponents menu

void *
OptionsMenuInit(void *prevMenu)
{
    if (MenuHandle) 
		return MenuHandle;

    MenuHandle = GfuiScreenCreate((float*)NULL, NULL, NULL, NULL, (tfuiCallback)NULL, 1);

    void *param = GfuiMenuLoad("optionsmenu.xml");

    GfuiMenuCreateStaticControls(MenuHandle, param);
    
// SDW hack to get access to Monitor menu, doesn't have a defined position yet
#ifdef MonitorMenu
    GfuiMenuCreateButtonControl(MenuHandle, param, "display", NULL, onMonitorMenuActivate);
#else
    GfuiMenuCreateButtonControl(MenuHandle, param, "display", NULL, onDisplayMenuActivate);
#endif
    GfuiMenuCreateButtonControl(MenuHandle, param, "sound", NULL, onSoundMenuActivate);

    GfuiMenuCreateButtonControl(MenuHandle, param, "back", prevMenu, GfuiScreenActivate);

    GfuiMenuCreateButtonControl(MenuHandle, param, "configure", NULL, onControlMenuActivate);

    GfParmReleaseHandle(param);

    GfuiMenuDefaultKeysAdd(MenuHandle);
    GfuiAddKey(MenuHandle, GFUIK_ESCAPE, "Back", prevMenu, GfuiScreenActivate, NULL);

    return MenuHandle;
}
