#include <tgfclient.h>
#include "legacymenu.h"
#include "mainmenu.h"
#include "DeveloperMenu.h"

static void *s_scrHandle = NULL;
static void *s_prevHandle = NULL;

static void OnActivate(void * /* dummy */)
{
    // add whatever needs to be done when the menu is opened here
}

void *DeveloperMenuInit(void *prevMenu)
{
    // screen already created
    if (s_scrHandle)
    {
        return s_scrHandle;
    }

    s_scrHandle = GfuiScreenCreate((float *)NULL, NULL, OnActivate, NULL,
                                   (tfuiCallback)NULL, 1);
    s_prevHandle = prevMenu;

    void *param = GfuiMenuLoad("DeveloperMenu.xml");
    GfuiMenuCreateStaticControls(s_scrHandle, param);
    // add UI button controls here

    GfParmReleaseHandle(param);
    // add keyboard controls here

    return s_scrHandle;
}