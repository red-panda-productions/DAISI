//
// Created by Stefan Hoekzema on 2/26/2022.
//

#include <tgfclient.h>

#include "legacymenu.h"
#include "InterventionType.h"

#include "interventionmenu.h"


/* Available intervention names and associated values */
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
static void	*ScrHandle = NULL;
static void	*PrevHandle = NULL;

/* Load the intervention type from the backend config */
static void ReadInterventionType(void)
{
    // TODO Read current intervention type from config

    GfuiLabelSetText(ScrHandle, InterventionTypeId, InterventionTypes[CurInterventionTypeIndex]);
}


/* Save the chosen intervention type into the backend config */
static void SaveInterventionType(void * /* dummy */)
{
    // TODO Change intervention in config here

    /* return to previous screen */
    GfuiScreenActivate(PrevHandle);

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
InterventionMenuInit(void *prevMenu)
{
    /* screen already created */
    if (ScrHandle) {
        return ScrHandle;
    }
    PrevHandle = prevMenu;

    ScrHandle = GfuiScreenCreate((float*)NULL, NULL, onActivate, NULL, (tfuiCallback)NULL, 1);

    void *param = GfuiMenuLoad("interventionmenu.xml");
    GfuiMenuCreateStaticControls(ScrHandle, param);

    GfuiMenuCreateButtonControl(ScrHandle,param,"interventionleftarrow",(void*)-1,ChangeInterventionType);
    GfuiMenuCreateButtonControl(ScrHandle,param,"interventionrightarrow",(void*)1,ChangeInterventionType);

    InterventionTypeId = GfuiMenuCreateLabelControl(ScrHandle,param,"interventionlabel");
    GfuiMenuCreateButtonControl(ScrHandle,param,"ApplyButton",prevMenu,SaveInterventionType);
    GfuiMenuCreateButtonControl(ScrHandle,param,"CancelButton",prevMenu,GfuiScreenActivate);

    GfParmReleaseHandle(param);

    GfuiAddKey(ScrHandle, GFUIK_RETURN, "Apply", NULL, SaveInterventionType, NULL);
    GfuiAddKey(ScrHandle, GFUIK_ESCAPE, "Cancel", prevMenu, GfuiScreenActivate, NULL);
    GfuiAddKey(ScrHandle, GFUIK_F1, "Help", ScrHandle, GfuiHelpScreen, NULL);
    GfuiAddKey(ScrHandle, GFUIK_F12, "Screen-Shot", NULL, GfuiScreenShot, NULL);
    GfuiAddKey(ScrHandle, GFUIK_LEFT, "Previous Skill Level", (void*)-1, ChangeInterventionType, NULL);
    GfuiAddKey(ScrHandle, GFUIK_RIGHT, "Next Skill Level", (void*)+1, ChangeInterventionType, NULL);

    return ScrHandle;
}
