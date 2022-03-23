#include <tgfclient.h>
#include <random>
#include <iostream>
#include "legacymenu.h"
#include "Mediator.h"
#include "ResearcherMenu.h"


// GUI screen handles
static void* s_scrHandle = NULL;
static void* s_nextHandle = NULL;

// Task
Task m_task = TASK_NO_TASK;

// Indicators
bool* m_indicators = new bool[2];

// InterventionType
InterventionType m_interventionType;

// Environment
Track m_track;

// Participant control
bool* m_pControl = new bool[3];

// Max time
int m_maxTime = 10.0f;
int m_maxTimeId;

// User ID
char* m_userID;
int m_userIDId;

/// @brief Sets the defaults values
static void OnActivate(void* /* dummy */)
{
    // Set standard Task
    m_task = TASK_NO_TASK;

    // Set standard indicators
    bool indicators[] = {false, false};
    m_indicators = indicators;

    // Set standard interventionType
    m_interventionType = INTERVENTION_TYPE_NO_SIGNALS;

    // Set standard player control settings
    bool pControl[] = {false, true, true};
    m_pControl = pControl;

    // Set standard max time
    char buf[32];
    sprintf(buf, "%d", m_maxTime);
    GfuiEditboxSetString(s_scrHandle, m_maxTimeId, buf);

    // Create random userID
    std::random_device rd;
    static std::default_random_engine generator(rd());
    std::uniform_int_distribution<int> distribution(1, 999999999);
    sprintf(buf, "%d", distribution(generator));

    // Set default userID
    m_userID = buf;
    GfuiEditboxSetString(s_scrHandle, m_userIDId, buf);
}


/// @brief        Sets the task to lane keeping
/// @param p_info Information on the checkbox
static void SelectLaneKeeping(tCheckBoxInfo* p_info)
{
    if (p_info->bChecked)
    {
        m_task = TASK_LANE_KEEPING;
    }
    else if (m_task == TASK_LANE_KEEPING)
    {
        // this is temporary, TODO: Make it so that only one checkbox can be selected at once
        m_task = TASK_NO_TASK;
    }
}
/// @brief        Sets the task to speed control
/// @param p_info Information on the checkbox
static void SelectSpeedControl(tCheckBoxInfo* p_info)
{
    if (p_info->bChecked)
    {
        m_task = TASK_SPEED_CONTROL;
    }
    else if (m_task == TASK_SPEED_CONTROL)
    {
        // this is temporary, TODO: Make it so that only one checkbox can be selected at once
        m_task = TASK_NO_TASK;
    }
}
/// @brief        Enables/disables the auditory indication for interventions
/// @param p_info Information on the checkbox
static void SelectAuditory(tCheckBoxInfo* p_info)
{
    m_indicators[INDICATOR_AUDITORY] = p_info->bChecked;
}
/// @brief        Enables/disables the visual indication for interventions
/// @param p_info Information on the checkbox
static void SelectVisual(tCheckBoxInfo* p_info)
{
    m_indicators[INDICATOR_VISUAL] = p_info->bChecked;
}
/// @brief        Sets the interventionType to no signals
/// @param p_info Information on the checkbox
static void SelectTypeNoSignals(tCheckBoxInfo* p_info)
{
    if (p_info->bChecked)
    {
        m_interventionType = INTERVENTION_TYPE_NO_SIGNALS;
    }
}
/// @brief        Sets the interventionType to only signals
/// @param p_info Information on the checkbox
static void SelectTypeOnlySignals(tCheckBoxInfo* p_info)
{
    if (p_info->bChecked)
    {
        m_interventionType = INTERVENTION_TYPE_ONLY_SIGNALS;
    }
}
/// @brief        Sets the interventionType to shared control
/// @param p_info Information on the checkbox
static void SelectTypeSharedControl(tCheckBoxInfo* p_info)
{
    if (p_info->bChecked)
    {
        m_interventionType = INTERVENTION_TYPE_SHARED_CONTROL;
    }
}
/// @brief        Sets the interventionType to complete takeover
/// @param p_info Information on the checkbox
static void SelectTypeCompleteTakeover(tCheckBoxInfo* p_info)
{
    if (p_info->bChecked)
    {
        m_interventionType = INTERVENTION_TYPE_COMPLETE_TAKEOVER;
    }
}
/// @brief        Sets the environment to highway
/// @param p_info Information on the checkbox
static void SelectEnvironmentHighway(tCheckBoxInfo* p_info)
{
    if (p_info->bChecked)
    {
        // TODO: move track selection from SD somewhere to here?
    }
}
/// @brief        Enables/disables the possibility for participants to enable/disable interventions
/// @param p_info Information on the checkbox
static void SelectControlInterventionOnOff(tCheckBoxInfo* p_info)
{
    m_pControl[PARTICIPANT_CONTROL_INTERVENTIONS_ON_OFF] = p_info->bChecked;
}
/// @brief        Enables/disables the possibility for participants to control gas
/// @param p_info Information on the checkbox
static void SelectControlGas(tCheckBoxInfo* p_info)
{
    m_pControl[PARTICIPANT_CONTROL_GAS] = p_info->bChecked;
}
/// @brief        Enables/disables the possibility for participants to control steering
/// @param p_info Information on the checkbox
static void SelectControlSteering(tCheckBoxInfo* p_info)
{
    m_pControl[PARTICIPANT_CONTROL_STEERING] = p_info->bChecked;
}

/// @brief Handle input in the max time textbox
static void SetMaxTime(void*)
{
    char* val = GfuiEditboxGetString(s_scrHandle, m_maxTimeId);
    sscanf(val, "%d", &m_maxTime);
    if (m_maxTime > 1440.0f)
        m_maxTime = 1440.0f;
    else if (m_maxTime < 0.0f)
        m_maxTime = 0.0f;

    char buf[32];
    sprintf(buf, "%d", m_maxTime);
    GfuiEditboxSetString(s_scrHandle, m_maxTimeId, buf);
}

/// @brief Handle input in the UserID textbox
static void SetUserID(void*)
{
    char* m_userID = GfuiEditboxGetString(s_scrHandle, m_userIDId);
    GfuiEditboxSetString(s_scrHandle, m_userIDId, m_userID);
}



/// @brief Saves the settings into the frontend settings and the backend config
static void SaveSettings(void* /* dummy */)
{
    // Save settings to backend config
    SMediator* mediator = SMediator::GetInstance();
    mediator->SetTask(m_task);
    mediator->SetIndicatorSettings(m_indicators);
    mediator->SetInterventionType(m_interventionType);
    mediator->SetMaxTime(m_maxTime);
    mediator->SetUserID(m_userID);

    // Save settings to frontend settings
    // TODO: Set Environment (Track)
    // TODO: Set Participant control (bool*)

    // Go to the next screen
    GfuiScreenActivate(s_nextHandle);
}

/// @brief            Initializes the researcher menu
/// @param p_nextMenu The scrHandle of the next menu
/// @return           The researcherMenu scrHandle
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

    void* param = GfuiMenuLoad("ResearcherMenu.xml");
    GfuiMenuCreateStaticControls(s_scrHandle, param);

    // Task checkboxes controls
    GfuiMenuCreateCheckboxControl(s_scrHandle, param, "CheckboxTaskLaneKeeping", NULL, SelectLaneKeeping);
    GfuiMenuCreateCheckboxControl(s_scrHandle, param, "CheckboxTaskSpeedControl", NULL, SelectSpeedControl);

    // Indicator checkboxes controls
    GfuiMenuCreateCheckboxControl(s_scrHandle, param, "CheckboxIndicatorAuditory", NULL, SelectAuditory);
    GfuiMenuCreateCheckboxControl(s_scrHandle, param, "CheckboxIndicatorVisual", NULL, SelectVisual);

    // Types checkboxes controls
    GfuiMenuCreateCheckboxControl(s_scrHandle, param, "CheckboxTypeNoSignals", NULL, SelectTypeNoSignals);
    GfuiMenuCreateCheckboxControl(s_scrHandle, param, "CheckboxTypeOnlySignals", NULL, SelectTypeOnlySignals);
    GfuiMenuCreateCheckboxControl(s_scrHandle, param, "CheckboxTypeSharedControl", NULL, SelectTypeSharedControl);
    GfuiMenuCreateCheckboxControl(s_scrHandle, param, "CheckboxTypeCompleteTakeover", NULL, SelectTypeCompleteTakeover);

    // Environment checkboxes controls
    GfuiMenuCreateCheckboxControl(s_scrHandle, param, "CheckboxEnvironmentHighway", NULL, SelectEnvironmentHighway);

    // Participant-Control checkboxes controls
    GfuiMenuCreateCheckboxControl(s_scrHandle, param, "CheckboxPControlInterventionToggle", NULL, SelectControlInterventionOnOff);
    GfuiMenuCreateCheckboxControl(s_scrHandle, param, "CheckboxPControlGas", NULL, SelectControlGas);
    GfuiMenuCreateCheckboxControl(s_scrHandle, param, "CheckboxPControlSteering", NULL, SelectControlSteering);

    // Textbox controls
    m_maxTimeId = GfuiMenuCreateEditControl(s_scrHandle, param, "MaxTimeEdit", NULL, NULL, SetMaxTime);
    m_userIDId  = GfuiMenuCreateEditControl(s_scrHandle, param, "UserIdEdit", NULL, NULL, SetUserID);

    // ApplyButton control
    GfuiMenuCreateButtonControl(s_scrHandle, param, "ApplyButton", s_scrHandle, SaveSettings);

    GfParmReleaseHandle(param);

    // Keyboard button controls
    GfuiAddKey(s_scrHandle, GFUIK_RETURN, "Apply", NULL, SaveSettings, NULL);
    GfuiAddKey(s_scrHandle, GFUIK_F1, "Help", s_scrHandle, GfuiHelpScreen, NULL);
    GfuiAddKey(s_scrHandle, GFUIK_F12, "Screen-Shot", NULL, GfuiScreenShot, NULL);

    return s_scrHandle;
}

/// @brief  Activates the researcher menu screen
/// @return 0 if successful, otherwise -1
int ResearcherMenuRun(void)
{
    GfuiScreenActivate(s_scrHandle);

    return 0;
}
