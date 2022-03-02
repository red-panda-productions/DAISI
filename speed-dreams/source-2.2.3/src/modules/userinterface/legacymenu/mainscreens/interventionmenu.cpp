//
// Created by Stefan Hoekzema on 2/26/2022.
//

#include <tgfclient.h>

#include "legacymenu.h"
#include "ConfigEnums.h"
#include "mainmenu.h"
#include "Mediator.h"

#include "interventionmenu.h"


/* Available intervention names and associated values {0,1,2,3,4} */
static const char *InterventionTypes[] = { "no intervention",
                                           "show intervention",
                                           "ask for intervention",
                                           "intervene when needed",
                                           "always intervene" };
static const int InterventionValues[] = { INTERVENTION_TYPE_NO_INTERVENTION,
                                          INTERVENTION_TYPE_INDICATION,
                                          INTERVENTION_TYPE_ASK_FOR,
                                          INTERVENTION_TYPE_PERFORM_WHEN_NEEDED,
                                          INTERVENTION_TYPE_ALWAYS_INTERVENE };
static const int NInterventions = sizeof(InterventionTypes) / sizeof(InterventionTypes[0]);
static int CurInterventionTypeIndex = 0;

/* GUI label ids */
static int InterventionTypeId;

/* GUI screen handles */
static void *MenuHandle = 0;
static void	*ScrHandle = NULL;
static void	*NextHandle = NULL;

/* Mediator to handle communication between front-end and back-end */
Mediator mediator;

/* Load the intervention type from the backend config */
static void ReadInterventionType(void)
{
    CurInterventionTypeIndex = mediator.GetInterventionType();

    GfuiLabelSetText(ScrHandle, InterventionTypeId, InterventionTypes[CurInterventionTypeIndex]);
}


/* Save the chosen intervention type into the backend config */
static void SaveInterventionType(void * /* dummy */)
{
    mediator.SetInterventionType(CurInterventionTypeIndex);

    /* go to the next screen */
    GfuiScreenActivate(NextHandle);

    return;
}

/* Change the intervention type */
static void
ChangeInterventionType(void *vp)
{
    const int delta = ((long)vp < 0) ? -1 : 1;

    CurInterventionTypeIndex = (CurInterventionTypeIndex + delta + NInterventions) % NInterventions;

    GfuiLabelSetText(ScrHandle, InterventionTypeId, InterventionTypes[CurInterventionTypeIndex]);
}


static void onActivate(void * /* dummy */)
{
    ReadInterventionType();
}


/* Menu creation */
void *
InterventionMenuInit(void* nextMenu)
{
    /* screen already created */
    if (ScrHandle) {
        return ScrHandle;
    }

    ScrHandle = GfuiScreenCreate((float*)NULL, NULL, onActivate, NULL, (tfuiCallback)NULL, 1);
    NextHandle = nextMenu;

    void *param = GfuiMenuLoad("interventionmenu.xml");
    GfuiMenuCreateStaticControls(ScrHandle, param);

    GfuiMenuCreateButtonControl(ScrHandle,param,"interventionleftarrow",(void*)-1,ChangeInterventionType);
    GfuiMenuCreateButtonControl(ScrHandle,param,"interventionrightarrow",(void*)1,ChangeInterventionType);

    InterventionTypeId = GfuiMenuCreateLabelControl(ScrHandle,param,"interventionlabel");
    GfuiMenuCreateButtonControl(ScrHandle,param,"ApplyButton",ScrHandle,SaveInterventionType);

    GfParmReleaseHandle(param);

    GfuiAddKey(ScrHandle, GFUIK_RETURN, "Apply", NULL, SaveInterventionType, NULL);
    GfuiAddKey(ScrHandle, GFUIK_F1, "Help", ScrHandle, GfuiHelpScreen, NULL);
    GfuiAddKey(ScrHandle, GFUIK_F12, "Screen-Shot", NULL, GfuiScreenShot, NULL);
    GfuiAddKey(ScrHandle, GFUIK_LEFT, "Previous Skill Level", (void*)-1, ChangeInterventionType, NULL);
    GfuiAddKey(ScrHandle, GFUIK_RIGHT, "Next Skill Level", (void*)+1, ChangeInterventionType, NULL);

    return ScrHandle;
}

/*
 * Function
 *	MainMenuRun
 * Description
 *	Activate the main menu
 * Parameters
 *	none
 * Returns
 *	0 ok -1 not ok
 */
int
InterventionMenuRun(void)
{
    GfuiScreenActivate(ScrHandle);

    return 0;
}
