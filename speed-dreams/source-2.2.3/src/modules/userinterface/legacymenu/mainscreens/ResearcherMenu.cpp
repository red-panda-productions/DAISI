#include <tgfclient.h>
#include <random>
#include <forcefeedback.h>

#include "legacymenu.h"
#include "Mediator.h"
#include "ResearcherMenu.h"


// GUI screen handles
static void* s_scrHandle  = nullptr;
static void* s_nextHandle = nullptr;

// Task
Task m_task = TASK_NO_TASK;

// Indicators
tIndicator m_indicators = { true, true, true };

// InterventionType
InterventionType m_interventionType;

// Environment
Track m_track;

// Participant control
tParticipantControl m_pControl = { false, true, true, true, false, false };

// Force feedback manager
extern TGFCLIENT_API ForceFeedbackManager forceFeedback;

// Max time
int m_maxTime = 10.0f;
int m_maxTimeControl;

// User ID
char m_userId[32];
int  m_userIdControl;


static void OnActivate(void* /* dummy */) { }

/// @brief        Sets the task to the selected one
/// @param p_info Information on the radio button pressed
static void SelectTask(tRadioButtonInfo* p_info)
{
    switch(p_info->Selected)
    {
        case 1:
            m_task = TASK_LANE_KEEPING;
            break;
        case 2:
            m_task = TASK_SPEED_CONTROL;
            break;
        default:
            m_task = TASK_NO_TASK;
            break;
    }
}

/// @brief        Enables/disables the auditory indication for interventions
/// @param p_info Information on the checkbox
static void SelectAudio(tCheckBoxInfo* p_info)
{
    m_indicators.Audio = p_info->bChecked;
}

/// @brief        Enables/disables the visual indication for interventions
/// @param p_info Information on the checkbox
static void SelectIcon(tCheckBoxInfo* p_info)
{
    m_indicators.Icon = p_info->bChecked;
}

/// @brief        Enables/disables the textual indication for interventions
/// @param p_info Information on the checkbox
static void SelectText(tCheckBoxInfo* p_info)
{
    m_indicators.Text = p_info->bChecked;
}

/// @brief        Sets the interventionType to the selected one
/// @param p_info Information on the radio button pressed
static void SelectInterventionType(tRadioButtonInfo* p_info)
{
    switch(p_info->Selected)
    {
        case 1:
            m_interventionType = INTERVENTION_TYPE_ONLY_SIGNALS;
            break;
        case 2:
            m_interventionType = INTERVENTION_TYPE_SHARED_CONTROL;
            break;
        case 3:
            m_interventionType = INTERVENTION_TYPE_COMPLETE_TAKEOVER;
            break;
        default:
            m_interventionType = INTERVENTION_TYPE_NO_SIGNALS;
            break;
    }
}

/// @brief        Sets the environment to the selected one
/// @param p_info Information on the radio button pressed
static void SelectEnvironment(tRadioButtonInfo* p_info)
{
    switch(p_info->Selected)
    {
        default:
            // TODO: set environment
            break;
    }
}

/// @brief        Enables/disables the possibility for participants to enable/disable interventions
/// @param p_info Information on the checkbox
static void SelectControlInterventionOnOff(tCheckBoxInfo* p_info)
{
    m_pControl.ControlInterventionToggle = p_info->bChecked;
}

/// @brief        Enables/disables the possibility for participants to control gas
/// @param p_info Information on the checkbox
static void SelectControlGas(tCheckBoxInfo* p_info)
{
    m_pControl.ControlGas = p_info->bChecked;
}

/// @brief        Enables/disables the possibility for participants to control steering
/// @param p_info Information on the checkbox
static void SelectControlSteering(tCheckBoxInfo* p_info)
{
    m_pControl.ControlSteering = p_info->bChecked;
}

/// @brief        Enables/disables the possibility for participants to control steering
/// @param p_info Information on the checkbox
static void SelectForceFeedback(tCheckBoxInfo* p_info)
{
    m_pControl.ForceFeedback = p_info->bChecked;
}

/// @brief        Enables/disables the simulation recorder
/// @param p_info Information on the checkbox
static void SelectRecorderOnOff(tCheckBoxInfo* p_info)
{
    m_pControl.RecordSession = p_info->bChecked;
}

/// @brief        Enables/disables the black box recorder
/// @param p_info Information on the checkbox
static void SelectBBRecorderOnOff(tCheckBoxInfo* p_info)
{
    m_pControl.BBRecordSession = p_info->bChecked;
}

/// @brief Handle input in the max time textbox
static void SetMaxTime(void*)
{
    char* val = GfuiEditboxGetString(s_scrHandle, m_maxTimeControl);
    sscanf(val, "%d", &m_maxTime);
    if (m_maxTime > 1440.0f)
        m_maxTime = 1440.0f;
    else if (m_maxTime < 0.0f)
        m_maxTime = 0.0f;

    char buf[32];
    sprintf(buf, "%d", m_maxTime);
    GfuiEditboxSetString(s_scrHandle, m_maxTimeControl, buf);
}

/// @brief Handle input in the userId textbox
static void SetUserId(void*)
{
    strcpy(m_userId, GfuiEditboxGetString(s_scrHandle, m_userIdControl));
    GfuiEditboxSetString(s_scrHandle, m_userIdControl, m_userId);
}

/// @brief Saves the settings into the frontend settings and the backend config
static void SaveSettings(void* /* dummy */)
{
    // Save settings to the SDAConfig
    SMediator* mediator = SMediator::GetInstance();
    mediator->SetTask(m_task);
    mediator->SetIndicatorSettings(m_indicators);
    mediator->SetInterventionType(m_interventionType);
    mediator->SetMaxTime(m_maxTime);
    mediator->SetPControlSettings(m_pControl);

    // Save the encrypted userId in the SDAConfig
    size_t encryptedUserId = std::hash<std::string>{}(m_userId);
    sprintf(m_userId, "%zu", encryptedUserId);
    mediator->SetUserId(m_userId);

    // Set the simulation environment
    // TODO: Set Environment (Track)

    // Enable/Disable force feedback in the force feedback manager
    forceFeedback.effectsConfig["globalEffect"]["enabled"] = m_pControl.ForceFeedback;
    forceFeedback.saveConfiguration();

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

    // Task radiobutton controls
    GfuiMenuCreateRadioButtonListControl(s_scrHandle, param, "TaskRadioButtonList", NULL, SelectTask);

    // Indicator checkbox controls
    GfuiMenuCreateCheckboxControl(s_scrHandle, param, "CheckboxIndicatorAuditory", NULL, SelectAudio);
    GfuiMenuCreateCheckboxControl(s_scrHandle, param, "CheckboxIndicatorVisual",   NULL, SelectIcon);
    GfuiMenuCreateCheckboxControl(s_scrHandle, param, "CheckboxIndicatorTextual",  NULL, SelectText);

    // InterventionTypes radiobutton controls
    GfuiMenuCreateRadioButtonListControl(s_scrHandle, param, "InterventionTypeRadioButtonList", NULL, SelectInterventionType);

    // Environment checkbox controls
    GfuiMenuCreateRadioButtonListControl(s_scrHandle, param, "EnvironmentRadioButtonList", NULL, SelectEnvironment);

    // Participant-Control checkbox controls
    GfuiMenuCreateCheckboxControl(s_scrHandle, param, "CheckboxPControlInterventionToggle", NULL, SelectControlInterventionOnOff);
    GfuiMenuCreateCheckboxControl(s_scrHandle, param, "CheckboxPControlGas",      NULL, SelectControlGas);
    GfuiMenuCreateCheckboxControl(s_scrHandle, param, "CheckboxPControlSteering", NULL, SelectControlSteering);

    // Other options checkbox controls
    GfuiMenuCreateCheckboxControl(s_scrHandle, param, "CheckboxForceFeedback",    NULL, SelectForceFeedback);
    GfuiMenuCreateCheckboxControl(s_scrHandle, param, "CheckboxRecorderToggle",   NULL, SelectRecorderOnOff);
    GfuiMenuCreateCheckboxControl(s_scrHandle, param, "CheckboxBBRecorderToggle", NULL, SelectBBRecorderOnOff);

    // Textbox controls
    m_maxTimeControl = GfuiMenuCreateEditControl(s_scrHandle, param, "MaxTimeEdit", NULL, NULL, SetMaxTime);
    m_userIdControl  = GfuiMenuCreateEditControl(s_scrHandle, param, "UserIdEdit",  NULL, NULL, SetUserId);

    // ApplyButton control
    GfuiMenuCreateButtonControl(s_scrHandle, param, "ApplyButton", s_scrHandle, SaveSettings);

    GfParmReleaseHandle(param);

    // Keyboard button controls
    GfuiAddKey(s_scrHandle, GFUIK_RETURN, "Apply", NULL, SaveSettings, NULL);
    GfuiAddKey(s_scrHandle, GFUIK_F1, "Help", s_scrHandle, GfuiHelpScreen, NULL);
    GfuiAddKey(s_scrHandle, GFUIK_F12, "Screen-Shot", NULL, GfuiScreenShot, NULL);

    // Set standard Task
    m_task = TASK_NO_TASK;

    // Set standard interventionType
    m_interventionType = INTERVENTION_TYPE_NO_SIGNALS;

    // Set standard max time
    char buf[32];
    sprintf(buf, "%d", m_maxTime);
    GfuiEditboxSetString(s_scrHandle, m_maxTimeControl, buf);

    // Create random userId
    std::random_device rd;
    static std::default_random_engine generator(rd());
    std::uniform_int_distribution<int> distribution(1, 999999999);
    sprintf(m_userId, "%d", distribution(generator));

    // Set default userId
    GfuiEditboxSetString(s_scrHandle, m_userIdControl, m_userId);

    return s_scrHandle;
}

/// @brief  Activates the researcher menu screen
/// @return 0 if successful, otherwise -1
int ResearcherMenuRun(void)
{
    GfuiScreenActivate(s_scrHandle);

    return 0;
}
