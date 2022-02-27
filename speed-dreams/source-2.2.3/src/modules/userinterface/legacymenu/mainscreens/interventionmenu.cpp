//
// Created by Stefan Hoekzema on 2/26/2022.
//

#include <tgfclient.h>

#include "legacymenu.h"

#include "interventionmenu.h"

static void *MenuHandle;

static void
onAcceptExit(void * /* dummy */)
{
    LegacyMenu::self().quit();
}

/*
 * Function
 *	InterventionMenuInit
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
 *	It is a copy of exitMenuInit for now
 */

void* InterventionMenuInit(void *prevMenu)
{
    if (MenuHandle) {
        GfuiScreenRelease(MenuHandle);
    }

    MenuHandle = GfuiScreenCreate();

    void *param = GfuiMenuLoad("exitmenu.xml");

    GfuiMenuCreateStaticControls(MenuHandle, param);
    GfuiMenuCreateButtonControl(MenuHandle, param, "yesquit", NULL, onAcceptExit);
    GfuiMenuCreateButtonControl(MenuHandle, param, "nobacktogame", prevMenu, GfuiScreenActivate);

    GfParmReleaseHandle(param);

    GfuiMenuDefaultKeysAdd(MenuHandle);
    GfuiAddKey(MenuHandle, GFUIK_ESCAPE, "No, back to the game", prevMenu, GfuiScreenActivate, NULL);

    return MenuHandle;
}