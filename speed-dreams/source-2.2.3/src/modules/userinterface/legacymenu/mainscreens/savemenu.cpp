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

#include "Mediator.h"
#include "conformationmenu.h"
#include "savemenu.h"
#include "mainmenu.h"

static void *MenuHandle = nullptr;

static void
onAcceptExit(void * /* dummy */)
{
    SMediator::GetInstance()->SetSaveRaceToDatabase(true);
    LmRaceEngine().abortRace();  // Do cleanup to get back correct setup files
    LegacyMenu::self().quit();
}

static void
onRestartAccept(void * /* dummy */)
{
    SMediator::GetInstance()->SetSaveRaceToDatabase(true);
    LmRaceEngine().restartRace();
}

void *SaveMenuInit(void *p_prevMenu, int p_saveWayVersion)
{
    if (MenuHandle)
    {
        GfuiScreenRelease(MenuHandle);
    }

    MenuHandle = GfuiScreenCreate();

    void *param = GfuiMenuLoad("savemenu.xml");
    GfuiMenuCreateStaticControls(MenuHandle, param);
    GfuiMenuCreateButtonControl(MenuHandle, param, "dontsave", ConformationMenuInit(MenuHandle, p_saveWayVersion), GfuiScreenActivate);
    switch (p_saveWayVersion)
    {
        case EXIT:
        {
            GfuiMenuCreateButtonControl(MenuHandle, param, "yessave", nullptr, onAcceptExit);
            break;
        }
        case RESTART:
        {
            GfuiMenuCreateButtonControl(MenuHandle, param, "yessave", nullptr, onRestartAccept);
            break;
        }
        default:
        {
            throw std::runtime_error("incorrect 'p_saveWayVersion', have you defined the new option in conformationmenu.h?");
        }
    }
    GfParmReleaseHandle(param);

    GfuiMenuDefaultKeysAdd(MenuHandle);
    GfuiAddKey(MenuHandle, GFUIK_ESCAPE, "Wait, changed my mind", p_prevMenu, GfuiScreenActivate, nullptr);

    return MenuHandle;
}

void *SaveMenuInitRestart(void *prevMenu)
{
    if (MenuHandle)
    {
        GfuiScreenRelease(MenuHandle);
    }

    MenuHandle = GfuiScreenCreate();

    void *param = GfuiMenuLoad("savemenu.xml");
    GfuiMenuCreateStaticControls(MenuHandle, param);

    GfParmReleaseHandle(param);

    GfuiMenuDefaultKeysAdd(MenuHandle);
    GfuiAddKey(MenuHandle, GFUIK_ESCAPE, "Wait, changed my mind", prevMenu, GfuiScreenActivate, nullptr);

    return MenuHandle;
}