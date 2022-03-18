#include <tgfclient.h>
#include "legacymenu.h"
#include "Mediator.h"
#include "InterventionMenu.h"


// Available intervention names and associated values {0,1,2,3,4}
static const char *s_interventionTypes[] = { "no intervention",
                                           "show intervention",
                                           "ask for intervention",
                                           "intervene when needed",
                                           "always intervene" };
static const int s_nrInterventions = sizeof(s_interventionTypes) / sizeof(s_interventionTypes[0]);
static int s_curInterventionTypeIndex = 0;

// GUI label ids
static int s_interventionTypeId;

// GUI screen handles
static void	*s_scrHandle = NULL;
static void	*s_nextHandle = NULL;

/// @brief Loads the intervention type from the backend config
static void ReadInterventionType(void)
{
    s_curInterventionTypeIndex = SMediator::GetInstance()->GetInterventionType();

    GfuiLabelSetText(s_scrHandle, s_interventionTypeId, s_interventionTypes[s_curInterventionTypeIndex]);
}

/// @brief Saves the chosen intervention type into the backend config
static void SaveInterventionType(void * /* dummy */)
{
    SMediator::GetInstance()->SetInterventionType(s_curInterventionTypeIndex);

    /* go to the next screen */
    GfuiScreenActivate(s_nextHandle);
}

/// @brief Changes the interventionType selected and displayed on screen
/// @param index The index of the selected interventionType
static void ChangeInterventionType(void *index)
{
    // delta is 1 if the right arrow has been pressed and -1 if the left arrow has been pressed
    const int delta = ((long)index < 0) ? -1 : 1;

    s_curInterventionTypeIndex = (s_curInterventionTypeIndex + delta + s_nrInterventions) % s_nrInterventions;

    GfuiLabelSetText(s_scrHandle, s_interventionTypeId, s_interventionTypes[s_curInterventionTypeIndex]);
}

/// @brief Sets the current interventionType displayed in the options correctly
static void OnActivate(void * /* dummy */)
{
    ReadInterventionType();
}


/// @brief Initializes the intervention menu
/// @param nextMenu The scrHandle of the next menu
/// @return The interventionMenu scrHandle
void *InterventionMenuInit(void* nextMenu)
{
    // return if screen already created
    if (s_scrHandle) {
        return s_scrHandle;
    }

    // otherwise, create the screen
    s_scrHandle = GfuiScreenCreate((float*)NULL, NULL, OnActivate, NULL, (tfuiCallback)NULL, 1);
    s_nextHandle = nextMenu;

    void *param = GfuiMenuLoad("interventionmenu.xml");
    GfuiMenuCreateStaticControls(s_scrHandle, param);

    // menu button controls: arrow buttons, label hover-ability and apply button
    GfuiMenuCreateButtonControl(s_scrHandle,param,"interventionleftarrow",(void*)-1,ChangeInterventionType);
    GfuiMenuCreateButtonControl(s_scrHandle,param,"interventionrightarrow",(void*)1,ChangeInterventionType);
    s_interventionTypeId = GfuiMenuCreateLabelControl(s_scrHandle,param,"interventionlabel");
    GfuiMenuCreateButtonControl(s_scrHandle,param,"ApplyButton",s_scrHandle,SaveInterventionType);

    GfParmReleaseHandle(param);

    // keyboard button controls
    GfuiAddKey(s_scrHandle, GFUIK_RETURN, "Apply", NULL, SaveInterventionType, NULL);
    GfuiAddKey(s_scrHandle, GFUIK_F1, "Help", s_scrHandle, GfuiHelpScreen, NULL);
    GfuiAddKey(s_scrHandle, GFUIK_F12, "Screen-Shot", NULL, GfuiScreenShot, NULL);
    GfuiAddKey(s_scrHandle, GFUIK_LEFT, "Previous Intervention Type", (void*)-1, ChangeInterventionType, NULL);
    GfuiAddKey(s_scrHandle, GFUIK_RIGHT, "Next Intervention Type", (void*)+1, ChangeInterventionType, NULL);

    return s_scrHandle;
}

/// @brief Activates the intervention menu screen
/// @return 0 if successful, otherwise -1
int InterventionMenuRun(void)
{
    GfuiScreenActivate(s_scrHandle);

    return 0;
}
