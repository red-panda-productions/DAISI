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

static void *MenuHandle = nullptr;

static void
onAcceptExit(void * /* dummy */)
{
    SMediator::GetInstance()->SetSaveRaceToDatabase(false);
    LmRaceEngine().abortRace();  // Do cleanup to get back correct setup files
    LegacyMenu::self().quit();
}

static void
onAcceptRestart(void * /* dummy */)
{
    SMediator::GetInstance()->SetSaveRaceToDatabase(false);
    LmRaceEngine().restartRace();
}

void *ConformationMenuInit(void *p_prevMenu, int p_saveWayVersion)
{
    if (MenuHandle)
    {
        GfuiScreenRelease(MenuHandle);
    }

    MenuHandle = GfuiScreenCreate();

    void *param = GfuiMenuLoad("conformationmenu.xml");
    GfuiMenuCreateStaticControls(MenuHandle, param);
    switch (p_saveWayVersion)
    {
        case EXIT:
        {
            GfuiMenuCreateButtonControl(MenuHandle, param, "imsure", nullptr, onAcceptExit);
            break;
        }
        case RESTART:
        {
            GfuiMenuCreateButtonControl(MenuHandle, param, "imsure", nullptr, onAcceptRestart);
            break;
        }
        default:
        {
            throw std::runtime_error("incorrect 'p_saveWayVersion', have you defined the new option in conformationmenu.h?");
        }
    }
    GfuiMenuCreateButtonControl(MenuHandle, param, "imsure", nullptr, onAcceptExit);
    GfuiMenuCreateButtonControl(MenuHandle, param, "waitdontdelete", p_prevMenu, GfuiScreenActivate);

    GfParmReleaseHandle(param);

    GfuiMenuDefaultKeysAdd(MenuHandle);
    GfuiAddKey(MenuHandle, GFUIK_ESCAPE, "Wait, don't delete the data", p_prevMenu, GfuiScreenActivate, nullptr);

    return MenuHandle;
}
