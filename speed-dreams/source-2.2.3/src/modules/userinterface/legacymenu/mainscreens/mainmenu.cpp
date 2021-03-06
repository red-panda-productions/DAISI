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




#include "ConfigEnums.h"
#include "mainmenu.h"
#include "exitmenu.h"
#include "optionsmenu.h"
#include "creditsmenu.h"
#include "DataSelectionMenu.h"
#include "ResearcherMenu.h"

static void *MenuHandle = 0;

// DAISI: changed from race initialization to researcher menu initialization
static void
onResearcherSelectMenuActivate(void * /* dummy */)
{
    GfuiScreenActivate(ResearcherMenuInit(MenuHandle));
}

//static void
//onRaceWESelectMenuActivate(void * /* dummy */)
/*{
	GfuiScreenActivate(RmRaceWESelectInit(MenuHandle));
}*/

static void
onOptionsMenuActivate(void * /* dummy */)
{
    GfuiScreenActivate(OptionsMenuInit(MenuHandle));
}



static void
onCreditsMenuActivate(void * /* dummy */)
{
    CreditsMenuActivate(MenuHandle);
}

// DAISI: added parameters
static void
onExitMenuActivate(void * /*dummy*/)
{
    // just exit the game, don't ask if you want to save (replay) data
    GfuiScreenActivate(ExitMenuInit(MenuHandle));
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

void *
MainMenuInit(bool SupportsHumanDrivers)
{
    // Initialize only once.
    if (MenuHandle)
        return MenuHandle;

    MenuHandle = GfuiScreenCreate((float*)NULL, 
				    NULL, onMainMenuActivate, 
				    NULL, (tfuiCallback)NULL, 
				    1);

    void *menuDescHdle = GfuiMenuLoad("mainmenu.xml");

    GfuiMenuCreateStaticControls(MenuHandle, menuDescHdle);

    //Add buttons and create based on xml
    // DAISI: changed button name from race to start
    GfuiMenuCreateButtonControl(MenuHandle, menuDescHdle, "start", NULL, onResearcherSelectMenuActivate);
    //GfuiMenuCreateButtonControl(MenuHandle, menuDescHdle, "weekend", NULL, onRaceWESelectMenuActivate);
    GfuiMenuCreateButtonControl(MenuHandle, menuDescHdle, "options", NULL, onOptionsMenuActivate);
    GfuiMenuCreateButtonControl(MenuHandle, menuDescHdle, "credits", NULL, onCreditsMenuActivate);


    GfuiMenuCreateButtonControl(MenuHandle, menuDescHdle, "quit", NULL, onExitMenuActivate);

    GfParmReleaseHandle(menuDescHdle);

    GfuiMenuDefaultKeysAdd(MenuHandle);
    GfuiAddKey(MenuHandle, GFUIK_ESCAPE, "Quit the game", NULL, onExitMenuActivate, NULL);

    return MenuHandle;
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
    GfuiScreenActivate(MenuHandle);
	
    return 0;
}
