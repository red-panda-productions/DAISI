/***************************************************************************

    file                 : savemenu.cpp
    created              : Tue Mar 18 10:54:12 CET 2022
    copyright            : (C) 2022 by Maik Vink
    email                : torcs@free.fr
    version              : $Id: savemenu.cpp $

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

#include "mediator.h"
#include "conformationmenu.h"
#include "mainmenu.h"

static void *MenuHandle = NULL;

static void
onAcceptExit(void * /* dummy */)
{
    SMediator::GetInstance()->SetSaveRaceToDatabase(false);
    LmRaceEngine().abortRace();  // Do cleanup to get back correct setup files
    LegacyMenu::self().quit();
}

static void
onRestartExit(void * /* dummy */)
{
    SMediator::GetInstance()->SetSaveRaceToDatabase(false);
    LmRaceEngine().restartRace();
}

void *ConformationMenuInit(void *prevMenu)
{
    if (MenuHandle)
    {
        GfuiScreenRelease(MenuHandle);
    }

    MenuHandle = GfuiScreenCreate();

    void *param = GfuiMenuLoad("conformationmenu.xml");
    GfuiMenuCreateStaticControls(MenuHandle, param);
    GfuiMenuCreateButtonControl(MenuHandle, param, "imsure", NULL, onAcceptExit);
    GfuiMenuCreateButtonControl(MenuHandle, param, "waitdontdelete", prevMenu, GfuiScreenActivate);

    GfParmReleaseHandle(param);

    GfuiMenuDefaultKeysAdd(MenuHandle);
    GfuiAddKey(MenuHandle, GFUIK_ESCAPE, "Wait, don't delete the data", prevMenu, GfuiScreenActivate, NULL);

    return MenuHandle;
}

void *ConformationMenuInitRestart(void *prevMenu)
{
    if (MenuHandle)
    {
        GfuiScreenRelease(MenuHandle);
    }

    MenuHandle = GfuiScreenCreate();

    void *param = GfuiMenuLoad("conformationmenu.xml");
    GfuiMenuCreateStaticControls(MenuHandle, param);
    GfuiMenuCreateButtonControl(MenuHandle, param, "imsure", NULL, onRestartExit);
    GfuiMenuCreateButtonControl(MenuHandle, param, "waitdontdelete", prevMenu, GfuiScreenActivate);

    GfParmReleaseHandle(param);

    GfuiMenuDefaultKeysAdd(MenuHandle);
    GfuiAddKey(MenuHandle, GFUIK_ESCAPE, "Wait, don't delete the data", prevMenu, GfuiScreenActivate, NULL);

    return MenuHandle;
}