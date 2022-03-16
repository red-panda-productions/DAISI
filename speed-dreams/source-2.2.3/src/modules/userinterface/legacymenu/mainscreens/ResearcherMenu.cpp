#include <tgfclient.h>
#include "legacymenu.h"
#include "Mediator.h"
#include "ResearcherMenu.h"

// Available intervention names and associated values {0,1,2,3,4}
static const char* s_interventionTypes[] = { "no intervention",
                                           "show intervention",
                                           "ask for intervention",
                                           "intervene when needed",
                                           "always intervene" };
static const int s_nrInterventions = sizeof(s_interventionTypes) / sizeof(s_interventionTypes[0]);
int m_curInterventionTypeIndex = 0;

// GUI label ids
static int s_interventionTypeId;

// GUI screen handles
static void* s_scrHandle = NULL;
static void* s_nextHandle = NULL;

/// @brief Loads the intervention type from the backend config
static void OnActivate(void* /* dummy */)
{
    m_curInterventionTypeIndex = Mediator::GetInstance().GetInterventionType();

    GfuiLabelSetText(s_scrHandle, s_interventionTypeId, s_interventionTypes[m_curInterventionTypeIndex]);
}

/// @brief Saves the settings into the backend config
static void SaveSettings(void* /* dummy */)
{
    Mediator::GetInstance().SetInterventionType(m_curInterventionTypeIndex);

    // Go to the next screen
    GfuiScreenActivate(s_nextHandle);
}

/// @brief Changes the interventionType selected and displayed on screen
/// @param index The index of the selected interventionType
static void ChangeInterventionType(void* p_index)
{
    // Delta is 1 if the right arrow has been pressed and -1 if the left arrow has been pressed
    const int delta = ((long)p_index < 0) ? -1 : 1;

    m_curInterventionTypeIndex = (m_curInterventionTypeIndex + delta + s_nrInterventions) % s_nrInterventions;

    GfuiLabelSetText(s_scrHandle, s_interventionTypeId, s_interventionTypes[m_curInterventionTypeIndex]);
}


/// @brief Initializes the researcher menu
/// @param nextMenu The scrHandle of the next menu
/// @return The researcherMenu scrHandle
void* ResearcherMenuInit(void* p_nextMenu)
{
    // Return if screen already created
    if (s_scrHandle) {
        return s_scrHandle;
    }

    // Otherwise, create the screen
    s_scrHandle = GfuiScreenCreate((float*)NULL, NULL, OnActivate,
                                   NULL, (tfuiCallback)NULL, 1);
    s_nextHandle = p_nextMenu;

    void *param = GfuiMenuLoad("ResearcherMenu.xml");
    GfuiMenuCreateStaticControls(s_scrHandle, param);

    // InterventionType controls: arrow buttons, label hover-ability
    GfuiMenuCreateButtonControl(s_scrHandle,param,"InterventionLeftArrow",(void*)-1,ChangeInterventionType);
    GfuiMenuCreateButtonControl(s_scrHandle,param,"InterventionRightArrow",(void*)1,ChangeInterventionType);
    s_interventionTypeId = GfuiMenuCreateLabelControl(s_scrHandle,param,"InterventionLabel");

    // ApplyButton control
    GfuiMenuCreateButtonControl(s_scrHandle, param, "ApplyButton", s_scrHandle, SaveSettings);

    GfParmReleaseHandle(param);

    // Keyboard button controls
    GfuiAddKey(s_scrHandle, GFUIK_RETURN, "Apply", NULL, SaveSettings, NULL);
    GfuiAddKey(s_scrHandle, GFUIK_F1, "Help", s_scrHandle, GfuiHelpScreen, NULL);
    GfuiAddKey(s_scrHandle, GFUIK_F12, "Screen-Shot", NULL, GfuiScreenShot, NULL);
    GfuiAddKey(s_scrHandle, GFUIK_LEFT, "Previous Intervention Type", (void*)-1, ChangeInterventionType, NULL);
    GfuiAddKey(s_scrHandle, GFUIK_RIGHT, "Next Intervention Type", (void*)+1, ChangeInterventionType, NULL);

    return s_scrHandle;
}

/// @brief Activates the researcher menu screen
/// @return 0 if successful, otherwise -1
int ResearcherMenuRun(void)
{
    GfuiScreenActivate(s_scrHandle);

    return 0;
}
