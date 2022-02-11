/***************************************************************************

    file                 : csnetclientsettings.cpp
    created              : Fri January 6 2017
    copyright            : (C) 2017 by Joe Thompson
    email                : beaglejoe@users.sourceforge.net
    version              : $Id: csnetclientsettings.cpp 6492 2017-01-10 04:51:14Z beaglejoe $

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#include <cstdlib>
#include <cstdio>
#include <cstring>

#include <portability.h>
#include <tgfclient.h>

#include <raceman.h>
#include <robot.h>

#include <racemanagers.h>
#include <race.h>

#include "racescreens.h"

#include "csnetworking.h"
#include <csnetwork.h>


// Menu variables.
static void* menuScreen;
static tRmNetworkSetting *MenuData;

// Menu control ids
static int rmcsIpEditId;
static int rmcsPortEditId;


// value variables
static int portNumber = SPEEDDREAMSPORT;
static std::string hostIP = "127.0.0.1";

extern std::string g_strHostIP;
extern std::string g_strHostPort;

static char	buf[512];

static void
loadOptions()
{
    snprintf(buf, sizeof(buf), "%s%s", GfLocalDir(), CS_PARAM_FILE);
    void* grHandle = GfParmReadFile(buf, GFPARM_RMODE_STD | GFPARM_RMODE_CREAT);

    portNumber = GfParmGetNum(grHandle, CS_SECT_CLIENT, CS_ATT_PORT, NULL, SPEEDDREAMSPORT);
    hostIP =  GfParmGetStr(grHandle, CS_SECT_CLIENT, CS_ATT_IP4, "127.0.0.1");
    // TODO

    GfParmReleaseHandle(grHandle);
}

static void
saveOptions()
{
    // Force current edit to loose focus (if one has it) and update associated variable.
    GfuiUnSelectCurrent();

    snprintf(buf, sizeof(buf), "%s%s", GfLocalDir(), CS_PARAM_FILE);
    void* grHandle = GfParmReadFile(buf, GFPARM_RMODE_STD | GFPARM_RMODE_CREAT);

    GfParmSetNum(grHandle, CS_SECT_CLIENT, CS_ATT_PORT, NULL, portNumber);
    GfParmSetStr(grHandle, CS_SECT_CLIENT, CS_ATT_IP4, hostIP.c_str());
    // TODO

    GfParmWriteFile(NULL, grHandle, "networking");

    GfParmReleaseHandle(grHandle);

    g_strHostIP = hostIP;
    snprintf(buf,sizeof(buf),"%ud",portNumber);
    g_strHostPort = buf;

}

static void
rmcsChangeIP(void* vp)
{
    if (vp)
    {
        char* val = GfuiEditboxGetString(menuScreen, rmcsIpEditId);
        if (val != NULL)
        {
            hostIP = val;
        }
    }

    // Display current value
    GfuiEditboxSetString(menuScreen, rmcsIpEditId, hostIP.c_str());
}

static void
rmcsChangePort(void* vp)
{
    if (vp)
    {
        char* val = GfuiEditboxGetString(menuScreen, rmcsPortEditId);
        if (val != NULL)
        {
            portNumber = strtol(val, (char **)NULL, 0);
        }
    }

    // Display current value
    snprintf(buf, sizeof(buf), "%d", portNumber);
    GfuiEditboxSetString(menuScreen, rmcsPortEditId, buf);
}

static void
rmcsActivate(void* /* dummy */)
{
    loadOptions();

    rmcsChangeIP(0);
    rmcsChangePort(0);

}

static void
rmcsDeactivate(void *screen)
{
    GfuiScreenRelease(menuScreen);
    
    if (screen) {
        GfuiScreenActivate(screen);
    }
}

static void
rmcsNext(void* nextScreenHandle)
{
    saveOptions();
    
    rmcsDeactivate(nextScreenHandle);
}

static void
rmcsAddKeys(void)
{
    GfuiMenuDefaultKeysAdd(menuScreen);
    GfuiAddKey(menuScreen, GFUIK_RETURN, "Accept", MenuData->nextScreen, rmcsNext, NULL);
    GfuiAddKey(menuScreen, GFUIK_ESCAPE, "Cancel", MenuData->prevScreen, rmcsDeactivate, NULL);
}

void
RmClientSettings(void *cs)
{
    MenuData = (tRmNetworkSetting*)cs;

    GfLogTrace("Entering Network Client Sttings menu.\n");

    // Create the screen, load menu XML descriptor and create static controls.
    menuScreen = GfuiScreenCreate(NULL, NULL, rmcsActivate, NULL, (tfuiCallback)NULL, 1);   
    void *menuXML = GfuiMenuLoad("csnetworkclientmenu.xml");
    GfuiMenuCreateStaticControls(menuScreen, menuXML);

    rmcsIpEditId = GfuiMenuCreateEditControl(menuScreen, menuXML, "IPAddrEdit", (void*)1, 0, rmcsChangeIP);
    GfuiEditboxSetString(menuScreen, rmcsIpEditId, g_strHostIP.c_str());

    rmcsPortEditId = GfuiMenuCreateEditControl(menuScreen, menuXML, "PortEdit", (void*)1, 0, rmcsChangePort);
    GfuiEditboxSetString(menuScreen, rmcsPortEditId, g_strHostPort.c_str());

    // Create the variable title label.
    int titleId = GfuiMenuCreateLabelControl(menuScreen, menuXML, "TitleLabel");
    std::string strTitle("Network options for ");
    strTitle += MenuData->pRace->getManager()->getName().c_str();
    GfuiLabelSetText(menuScreen, titleId, strTitle.c_str());

    // Create Next and Back buttons
    GfuiMenuCreateButtonControl(menuScreen, menuXML, "nextbutton",
                                MenuData->nextScreen, rmcsNext);
    GfuiMenuCreateButtonControl(menuScreen, menuXML, "backbutton",
                                MenuData->prevScreen, rmcsDeactivate);
    
    // Close menu XML descriptor.
    GfParmReleaseHandle(menuXML);
    
    // Register keyboard shortcuts.
    rmcsAddKeys();
    
    GfuiScreenActivate(menuScreen);
}
