/***************************************************************************

	file        : raceselectmenu.cpp
	created     : Sat Nov 16 09:36:29 CET 2002
	copyright   : (C) 2002 by Eric Espie
	email       : eric.espie@torcs.org
	version     : $Id: raceselectmenu.cpp 7011 2020-04-30 15:23:52Z iobyte $

 ***************************************************************************/

 /***************************************************************************
  *                                                                         *
  *   This program is free software; you can redistribute it and/or modify  *
  *   it under the terms of the GNU General Public License as published by  *
  *   the Free Software Foundation; either version 2 of the License, or     *
  *   (at your option) any later version.                                   *
  *                                                                         *
  ***************************************************************************/

  /** @file
			  Race selection menu
	  @author	<a href=mailto:eric.espie@torcs.org>Eric Espie</a>
	  @version	$Id: raceselectmenu.cpp 7011 2020-04-30 15:23:52Z iobyte $
  */

#include <map>
#include <algorithm>

#include <tgfclient.h>

#include <raceman.h>

#include <racemanagers.h>
#include <race.h>

#include "legacymenu.h"
#include "racescreens.h"

#define SINGLE_RACEMAN_IDX 0 

  // The Race Select menu.
void* RmRaceSelectMenuHandle = NULL;


static std::map<std::string, int> rmMapSubTypeComboIds;

static void
rmOnSelectRaceMan()
{
	// Get the race managers with the given type
	const std::vector<std::string>& vecRaceManTypes = GfRaceManagers::self()->getTypes();
	const std::string strRaceManType = vecRaceManTypes[SINGLE_RACEMAN_IDX];
	const std::vector<GfRaceManager*> vecRaceMans =
		GfRaceManagers::self()->getRaceManagersWithType(strRaceManType);

	// If more than 1, get the one with the currently selected sub-type.
	GfRaceManager* pSelRaceMan = 0;
	if (vecRaceMans.size() > 1)
	{
		const int nSubTypeComboId = rmMapSubTypeComboIds[strRaceManType];
		const char* pszSelSubType = GfuiComboboxGetText(RmRaceSelectMenuHandle, nSubTypeComboId);
		std::vector<GfRaceManager*>::const_iterator itRaceMan;
		for (itRaceMan = vecRaceMans.begin(); itRaceMan != vecRaceMans.end(); ++itRaceMan)
		{
			if ((*itRaceMan)->getSubType() == pszSelSubType)
			{
				// Start configuring it.
				pSelRaceMan = *itRaceMan;
				break;
			}
		}
	}

	// If only 1, no choice.
	else if (vecRaceMans.size() == 1)
	{
		pSelRaceMan = vecRaceMans.back();
	}

	if (pSelRaceMan)
	{
		// Give the selected race manager to the race engine.
		LmRaceEngine().selectRaceman(pSelRaceMan);

		// Start the race configuration menus sequence.
		LmRaceEngine().configureRace(/* bInteractive */ true);
	}
	else
	{
		GfLogError("No such race manager (type '%s')\n", strRaceManType.c_str());
	}
}

/* Called when the menu is activated */
static void
rmOnActivate(void* dummy)
{
	GfLogTrace("Entering Race Mode Select menu\n");

	LmRaceEngine().reset();
	// SIMULATED DRIVING ASSISTANCE: skip this menu when activated
	// 
	rmOnSelectRaceMan();

}


/* Exit from Race engine */
static void
rmOnRaceSelectShutdown(void* prevMenu)
{
	GfuiScreenActivate(prevMenu);

	LmRaceEngine().cleanup();

	LegacyMenu::self().shutdownGraphics(/*bUnloadModule=*/true);
}




static void
rmOnChangeRaceMan(tComboBoxInfo*)
{
}

/* Initialize the single player menu */
void*
RmRaceSelectInit(void* prevMenu)
{
	if (RmRaceSelectMenuHandle)
		return RmRaceSelectMenuHandle;

	// Ask the RaceEngine what types of races should be allowed here
	bool SupportsHumanDrivers = LmRaceEngine().supportsHumanDrivers();

	// Create screen, load menu XML descriptor and create static controls.
	RmRaceSelectMenuHandle = GfuiScreenCreate((float*)NULL,
		NULL, rmOnActivate,
		NULL, (tfuiCallback)NULL,
		1);

	void* hMenuXMLDesc = GfuiMenuLoad("raceselectmenu.xml");

	GfuiMenuCreateStaticControls(RmRaceSelectMenuHandle, hMenuXMLDesc);

	

	// Create Back button
	GfuiMenuCreateButtonControl(RmRaceSelectMenuHandle, hMenuXMLDesc, "BackButton",
		prevMenu, rmOnRaceSelectShutdown);

	// Close menu XML descriptor.
	GfParmReleaseHandle(hMenuXMLDesc);

	// Register keyboard shortcuts.
	GfuiMenuDefaultKeysAdd(RmRaceSelectMenuHandle);
	GfuiAddKey(RmRaceSelectMenuHandle, GFUIK_ESCAPE, "Back To Main Menu",
		prevMenu, rmOnRaceSelectShutdown, NULL);

	// Give the race engine the menu to come back to.
	LmRaceEngine().initializeState(prevMenu);

	return RmRaceSelectMenuHandle;
}
