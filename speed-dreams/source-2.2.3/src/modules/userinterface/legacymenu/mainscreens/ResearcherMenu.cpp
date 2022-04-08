#include <tgfclient.h>
#include <random>
#include <forcefeedback.h>
#include "guimenu.h"
#include "legacymenu.h"
#include "Mediator.h"
#include "ResearcherMenu.h"

#define RESEARCHMENU_TASKS                       "dynamic controls/TaskRadioButtonList"
#define RESEARCHMENU_INDICATOR_AUDITORY          "dynamic controls/CheckboxIndicatorAuditory"
#define RESEARCHMENU_INDICATOR_VISUAL            "dynamic controls/CheckboxIndicatorVisual"
#define RESEARCHMENU_INDICATOR_TEXT              "dynamic controls/CheckboxIndicatorTextual"
#define RESEARCHMENU_INTERVENTIONTYPE            "dynamic controls/InterventionTypeRadioButtonList"
#define RESEARCHMENU_CONTROL_GAS                 "dynamic controls/CheckboxPControlGas"
#define RESEARCHMENU_CONTROL_INTERVENTION_TOGGLE "dynamic controls/CheckboxPControlInterventionToggle"
#define RESEARCHMENU_CONTROL_STEERING            "dynamic controls/CheckboxPControlSteering"
#define RESEARCHMENU_FORCE_FEEDBACK              "dynamic controls/CheckboxForceFeedback"
#define RESEARCHMENU_MAX_TIME_EDIT               "dynamic controls/MaxTimeEdit"

// GUI screen handles
static void* s_scrHandle  = nullptr;
static void* s_nextHandle = nullptr;

// GUI settings Id's 
int m_indicatorsGui[3];
int m_pControlGui[4];
int m_taskGui;
int m_interventionTypeGui;

// Task
Task m_task = TASK_NO_TASK;

// Indicators
tIndicator m_indicators = { true, true, true };

// InterventionType
InterventionType m_interventionType;

// Environment
Track m_track;

// Participant control
tParticipantControl m_pControl = { false, true, true, true };

// Force feedback manager
extern TGFCLIENT_API ForceFeedbackManager forceFeedback;

// Max time
int m_maxTime = 10.0f;
int m_maxTimeControl;

// User ID
char m_userId[32];
int  m_userIdControl;


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

/// @brief Saves the settings into the ResearcherMenu.xml file
static void SaveSettingsToDisk() 
{
    // Copies xml to documents folder and then ospens file parameter
    std::string dstStr("config/ResearcherMenu.xml");
    char dst[512];
    sprintf(dst, "%s%s", GfLocalDir(), dstStr.c_str());
    void* readParam = GfParmReadFile(dst, GFPARM_RMODE_REREAD | GFPARM_RMODE_CREAT);

    // Save task settings to xml file
    char val[32];
    sprintf(val, "%d", m_task);
    GfParmSetStr(readParam, RESEARCHMENU_TASKS, GFMNU_ATTR_SELECTED, val);

    // Save indicator settings to xml file
    const char* audioSetting = m_indicators.Audio ? GFMNU_VAL_YES : GFMNU_VAL_NO;
    const char* iconSetting  = m_indicators.Icon  ? GFMNU_VAL_YES : GFMNU_VAL_NO;
    const char* textSetting  = m_indicators.Text  ? GFMNU_VAL_YES : GFMNU_VAL_NO;
    GfParmSetStr(readParam, RESEARCHMENU_INDICATOR_AUDITORY, GFMNU_ATTR_CHECKED, audioSetting);
    GfParmSetStr(readParam, RESEARCHMENU_INDICATOR_VISUAL,   GFMNU_ATTR_CHECKED, iconSetting);
    GfParmSetStr(readParam, RESEARCHMENU_INDICATOR_TEXT,     GFMNU_ATTR_CHECKED, textSetting);

    // Save intervention type settings to xml file
    sprintf(val, "%d", m_interventionType);
    GfParmSetStr(readParam, RESEARCHMENU_INTERVENTIONTYPE, GFMNU_ATTR_SELECTED, val);

    // Save participant control settings to xml file
    const char* controlGas         = m_pControl.ControlGas                ? GFMNU_VAL_YES : GFMNU_VAL_NO;
    const char* interventionToggle = m_pControl.ControlInterventionToggle ? GFMNU_VAL_YES : GFMNU_VAL_NO;
    const char* controlSteering    = m_pControl.ControlSteering           ? GFMNU_VAL_YES : GFMNU_VAL_NO;
    const char* forceFeedback      = m_pControl.ForceFeedback             ? GFMNU_VAL_YES : GFMNU_VAL_NO;
    GfParmSetStr(readParam, RESEARCHMENU_CONTROL_GAS,                 GFMNU_ATTR_CHECKED, controlGas);
    GfParmSetStr(readParam, RESEARCHMENU_CONTROL_INTERVENTION_TOGGLE, GFMNU_ATTR_CHECKED, interventionToggle);
    GfParmSetStr(readParam, RESEARCHMENU_CONTROL_STEERING,            GFMNU_ATTR_CHECKED, controlSteering);
    GfParmSetStr(readParam, RESEARCHMENU_FORCE_FEEDBACK,              GFMNU_ATTR_CHECKED, forceFeedback);

    // Save max time to xml file
    char buf[32];
    sprintf(buf, "%d", m_maxTime);
    GfParmSetStr(readParam, RESEARCHMENU_MAX_TIME_EDIT, GFMNU_ATTR_DEFAULT_VALUE, buf);

    // Write all the above queued changed to xml file
    GfParmWriteFile(NULL, readParam, "ResearcherMenu");
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

    // Save the encrypted userId in the SDAConfig
    size_t encryptedUserId = std::hash<std::string>{}(m_userId);
    sprintf(m_userId, "%zu", encryptedUserId);
    mediator->SetUserId(m_userId);

    // Save settings in the ResearcherMenu.xml
    SaveSettingsToDisk();

    // Save settings to frontend settings
    // TODO: Set Environment (Track)
    // TODO: Set Participant control (tParticipantControl)

    // Enable/Disable force feedback in the force feedback manager
    forceFeedback.effectsConfig["globalEffect"]["enabled"] = m_pControl.ForceFeedback;
    forceFeedback.saveConfiguration();

    // Go to the next screen
    GfuiScreenActivate(s_nextHandle);
}

/// @brief                    Sets all the checkboxes and radiobuttons in the researcher menu
void InitializeMenuButtons()
{
    GfuiCheckboxSetChecked(s_scrHandle, m_indicatorsGui[0], m_indicators.Audio);
    GfuiCheckboxSetChecked(s_scrHandle, m_indicatorsGui[1], m_indicators.Icon);
    GfuiCheckboxSetChecked(s_scrHandle, m_indicatorsGui[2], m_indicators.Audio);
    GfuiCheckboxSetChecked(s_scrHandle, m_pControlGui[0], m_pControl.ControlInterventionToggle);
    GfuiCheckboxSetChecked(s_scrHandle, m_pControlGui[1], m_pControl.ControlGas);
    GfuiCheckboxSetChecked(s_scrHandle, m_pControlGui[2], m_pControl.ControlSteering);
    GfuiCheckboxSetChecked(s_scrHandle, m_pControlGui[3], m_pControl.ForceFeedback);
    GfuiRadioButtonListSetSelected(s_scrHandle, m_taskGui, m_task);
    GfuiRadioButtonListSetSelected(s_scrHandle, m_interventionTypeGui, m_interventionType);
}

/// @brief         Initializes the menu setting from the ResearcherMenu.xml file
/// @param p_param The configuration menu handle
void InitializeSettings(void* p_param)
{
    // Retrieve all setting variables from the xml file and assigning them to the internal variables
    m_task = std::stoi(GfParmGetStr(p_param, RESEARCHMENU_TASKS, GFMNU_ATTR_SELECTED, NULL));

    m_indicators.Audio = gfuiMenuGetBoolean(GfParmGetStr(p_param, RESEARCHMENU_INDICATOR_AUDITORY, GFMNU_ATTR_CHECKED, NULL), true);
    m_indicators.Icon = gfuiMenuGetBoolean(GfParmGetStr(p_param, RESEARCHMENU_INDICATOR_VISUAL, GFMNU_ATTR_CHECKED, NULL), true);
    m_indicators.Text = gfuiMenuGetBoolean(GfParmGetStr(p_param, RESEARCHMENU_INDICATOR_TEXT, GFMNU_ATTR_CHECKED, NULL), true);

    m_interventionType = std::stoi(GfParmGetStr(p_param, RESEARCHMENU_INTERVENTIONTYPE, GFMNU_ATTR_SELECTED, NULL));

    m_pControl.ControlGas = gfuiMenuGetBoolean(GfParmGetStr(p_param, RESEARCHMENU_CONTROL_GAS, GFMNU_ATTR_CHECKED, NULL), true);
    m_pControl.ControlInterventionToggle = gfuiMenuGetBoolean(GfParmGetStr(p_param, RESEARCHMENU_CONTROL_INTERVENTION_TOGGLE, GFMNU_ATTR_CHECKED, NULL), false);
    m_pControl.ControlSteering = gfuiMenuGetBoolean(GfParmGetStr(p_param, RESEARCHMENU_CONTROL_STEERING, GFMNU_ATTR_CHECKED, NULL), true);
    m_pControl.ForceFeedback = gfuiMenuGetBoolean(GfParmGetStr(p_param, RESEARCHMENU_FORCE_FEEDBACK, GFMNU_ATTR_CHECKED, NULL), true);

    // Set the max time setting from the xml file
    m_maxTime = std::stoi(GfParmGetStr(p_param, RESEARCHMENU_MAX_TIME_EDIT, GFMNU_ATTR_DEFAULT_VALUE, NULL));

    // Match the menu buttons with the initialized values / checking checkboxes and radiobuttons
    InitializeMenuButtons();
}



/// @brief Sets the defaults values
static void OnActivate(void* /* dummy */)
{
    // Retrieves the saved user xml file, if it doesn't exist the settings are already initialized in ResearcherMenuInit
    std::string strPath("config/ResearcherMenu.xml");
    char buf[512];
    sprintf(buf, "%s%s", GfLocalDir(), strPath.c_str());
    if (GfFileExists(buf)) {
        void* param = GfParmReadFile(buf, GFPARM_RMODE_STD);
        // Initialize settings with the retrieved xml file
        InitializeSettings(param);
    }
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


    // Task radio button controls
    m_taskGui = GfuiMenuCreateRadioButtonListControl(s_scrHandle, param, "TaskRadioButtonList", NULL, SelectTask);

    // Indicator checkboxes controls
    m_indicatorsGui[0] = GfuiMenuCreateCheckboxControl(s_scrHandle, param, "CheckboxIndicatorAuditory", NULL, SelectAudio);
    m_indicatorsGui[1] = GfuiMenuCreateCheckboxControl(s_scrHandle, param, "CheckboxIndicatorVisual", NULL, SelectIcon);
    m_indicatorsGui[2] = GfuiMenuCreateCheckboxControl(s_scrHandle, param, "CheckboxIndicatorTextual", NULL, SelectText);

    // InterventionTypes radio button controls
    m_interventionTypeGui = GfuiMenuCreateRadioButtonListControl(s_scrHandle, param, "InterventionTypeRadioButtonList", NULL, SelectInterventionType);

    // Environment checkboxes controls
    GfuiMenuCreateRadioButtonListControl(s_scrHandle, param, "EnvironmentRadioButtonList", NULL, SelectEnvironment);

    // Participant-Control checkboxes controls
    m_pControlGui[0] = GfuiMenuCreateCheckboxControl(s_scrHandle, param, "CheckboxPControlInterventionToggle", NULL, SelectControlInterventionOnOff);
    m_pControlGui[1] = GfuiMenuCreateCheckboxControl(s_scrHandle, param, "CheckboxPControlGas", NULL, SelectControlGas);
    m_pControlGui[2] = GfuiMenuCreateCheckboxControl(s_scrHandle, param, "CheckboxPControlSteering", NULL, SelectControlSteering);
    m_pControlGui[3] = GfuiMenuCreateCheckboxControl(s_scrHandle, param, "CheckboxForceFeedback", NULL, SelectForceFeedback);

    // Textbox controls
    m_maxTimeControl = GfuiMenuCreateEditControl(s_scrHandle, param, "MaxTimeEdit", NULL, NULL, SetMaxTime);
    m_userIdControl  = GfuiMenuCreateEditControl(s_scrHandle, param, "UserIdEdit", NULL, NULL, SetUserId);

    // ApplyButton control
    GfuiMenuCreateButtonControl(s_scrHandle, param, "ApplyButton", s_scrHandle, SaveSettings);

    GfParmReleaseHandle(param);

    // Keyboard button controls
    GfuiAddKey(s_scrHandle, GFUIK_RETURN, "Apply", NULL, SaveSettings, NULL);
    GfuiAddKey(s_scrHandle, GFUIK_F1, "Help", s_scrHandle, GfuiHelpScreen, NULL);
    GfuiAddKey(s_scrHandle, GFUIK_F12, "Screen-Shot", NULL, GfuiScreenShot, NULL);

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
