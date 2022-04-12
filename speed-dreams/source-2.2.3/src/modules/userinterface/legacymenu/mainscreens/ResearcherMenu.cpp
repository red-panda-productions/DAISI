#include <tgfclient.h>
#include <random>
#include <forcefeedback.h>
#include "guimenu.h"
#include "legacymenu.h"
#include "Mediator.h"
#include "ResearcherMenu.h"

// Parameters used in the xml files
#define PRM_TASKS            "TaskRadioButtonList"
#define PRM_INDCTR_AUDITORY  "CheckboxIndicatorAuditory"
#define PRM_INDCTR_VISUAL    "CheckboxIndicatorVisual"
#define PRM_INDCTR_TEXT      "CheckboxIndicatorTextual"
#define PRM_INTERVENTIONTYPE "InterventionTypeRadioButtonList"
#define PRM_ENVIRONMENT      "EnvironmentRadioButtonList"
#define PRM_CTRL_GAS         "CheckboxPControlGas"
#define PRM_CTRL_INTRV_TGGLE "CheckboxPControlInterventionToggle"
#define PRM_CTRL_STEERING    "CheckboxPControlSteering"
#define PRM_FORCE_FEEDBACK   "CheckboxForceFeedback"
#define PRM_RECORD_TGGLE     "CheckboxRecorderToggle"
#define PRM_RECORD_BB_TGGLE  "CheckboxBBRecorderToggle"
#define PRM_MAX_TIME         "MaxTimeEdit"
#define PRM_USER_ID          "UserIdEdit"

// GUI screen handles
static void* s_scrHandle  = nullptr;
static void* s_nextHandle = nullptr;

// GUI settings Id's 
int m_indicatorsControl[3];
int m_pControlControl[6];
int m_taskControl;
int m_interventionTypeControl;

// Task
Task m_task;

// Indicators
tIndicator m_indicators;

// InterventionType
InterventionType m_interventionType;

// Environment
Track m_track;

// Participant control
tParticipantControl m_pControl;

// Force feedback manager
extern TGFCLIENT_API ForceFeedbackManager forceFeedback;

// Max time
int m_maxTime;
int m_maxTimeControl;

// User ID
char m_userId[32];
int  m_userIdControl;


/// @brief        Sets the task to the selected one
/// @param p_info Information on the radio button pressed
static void SelectTask(tRadioButtonInfo* p_info)
{
    m_task = (Task)p_info->Selected;
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
    m_interventionType = (InterventionType)p_info->Selected;
}

/// @brief        Sets the environment to the selected one
/// @param p_info Information on the radio button pressed
static void SelectEnvironment(tRadioButtonInfo* p_info)
{
    // TODO: set environment
    // m_track = _something_
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

/// @brief Saves the settings into the ResearcherMenu.xml file
static void SaveSettingsToDisk() 
{
    // Copies xml to documents folder and then opens file parameter
    std::string dstStr("config/ResearcherMenu.xml");
    char dst[512];
    sprintf(dst, "%s%s", GfLocalDir(), dstStr.c_str());
    void* readParam = GfParmReadFile(dst, GFPARM_RMODE_REREAD | GFPARM_RMODE_CREAT);

    // Save task settings to xml file
    char val[32];
    sprintf(val, "%d", m_task);
    GfParmSetStr(readParam, PRM_TASKS, GFMNU_ATTR_SELECTED, val);

    // Save indicator settings to xml file
    GfParmSetStr(readParam, PRM_INDCTR_AUDITORY, GFMNU_ATTR_CHECKED, GfuiMenuBoolToStr(m_indicators.Audio));
    GfParmSetStr(readParam, PRM_INDCTR_VISUAL,   GFMNU_ATTR_CHECKED, GfuiMenuBoolToStr(m_indicators.Icon));
    GfParmSetStr(readParam, PRM_INDCTR_TEXT,     GFMNU_ATTR_CHECKED, GfuiMenuBoolToStr(m_indicators.Text));

    // Save intervention type settings to xml file
    sprintf(val, "%d", m_interventionType);
    GfParmSetStr(readParam, PRM_INTERVENTIONTYPE, GFMNU_ATTR_SELECTED, val);

    // Save participant control settings to xml file
    GfParmSetStr(readParam, PRM_CTRL_GAS,         GFMNU_ATTR_CHECKED, GfuiMenuBoolToStr(m_pControl.ControlGas));
    GfParmSetStr(readParam, PRM_CTRL_INTRV_TGGLE, GFMNU_ATTR_CHECKED, GfuiMenuBoolToStr(m_pControl.ControlInterventionToggle));
    GfParmSetStr(readParam, PRM_CTRL_STEERING,    GFMNU_ATTR_CHECKED, GfuiMenuBoolToStr(m_pControl.ControlSteering));

    GfParmSetStr(readParam, PRM_FORCE_FEEDBACK,   GFMNU_ATTR_CHECKED, GfuiMenuBoolToStr(m_pControl.ForceFeedback));
    GfParmSetStr(readParam, PRM_RECORD_TGGLE,     GFMNU_ATTR_CHECKED, GfuiMenuBoolToStr(m_pControl.RecordSession));
    GfParmSetStr(readParam, PRM_RECORD_BB_TGGLE,  GFMNU_ATTR_CHECKED, GfuiMenuBoolToStr(m_pControl.BBRecordSession));

    // Save max time to xml file
    char buf[32];
    sprintf(buf, "%d", m_maxTime);
    GfParmSetStr(readParam, PRM_MAX_TIME, GFMNU_ATTR_TEXT, buf);

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
    mediator->SetPControlSettings(m_pControl);

    // Save the encrypted userId in the SDAConfig
    size_t encryptedUserId = std::hash<std::string>{}(m_userId);
    sprintf(m_userId, "%zu", encryptedUserId);
    mediator->SetUserId(m_userId);

    // Save settings in the ResearcherMenu.xml
    SaveSettingsToDisk();

    // Save settings to frontend settings

    // TODO: Set Environment (Track)

    // Enable/Disable force feedback in the force feedback manager
    forceFeedback.effectsConfig["globalEffect"]["enabled"] = m_pControl.ForceFeedback;
    forceFeedback.saveConfiguration();

    // Go to the next screen
    GfuiScreenActivate(s_nextHandle);
}

/// @brief Synchronizes all the menu controls in the researcher menu to the internal variables
static void SynchronizeControls()
{
    GfuiRadioButtonListSetSelected(s_scrHandle, m_taskControl, m_task);

    GfuiCheckboxSetChecked(s_scrHandle, m_indicatorsControl[0], m_indicators.Audio);
    GfuiCheckboxSetChecked(s_scrHandle, m_indicatorsControl[1], m_indicators.Icon);
    GfuiCheckboxSetChecked(s_scrHandle, m_indicatorsControl[2], m_indicators.Text);

    GfuiRadioButtonListSetSelected(s_scrHandle, m_interventionTypeControl, m_interventionType);

    GfuiCheckboxSetChecked(s_scrHandle, m_pControlControl[0], m_pControl.ControlGas);
    GfuiCheckboxSetChecked(s_scrHandle, m_pControlControl[1], m_pControl.ControlInterventionToggle);
    GfuiCheckboxSetChecked(s_scrHandle, m_pControlControl[2], m_pControl.ControlSteering);
    GfuiCheckboxSetChecked(s_scrHandle, m_pControlControl[3], m_pControl.ForceFeedback);
    GfuiCheckboxSetChecked(s_scrHandle, m_pControlControl[4], m_pControl.RecordSession);
    GfuiCheckboxSetChecked(s_scrHandle, m_pControlControl[5], m_pControl.BBRecordSession);

    char buf[32];
    sprintf(buf, "%d", m_maxTime);
    GfuiEditboxSetString(s_scrHandle, m_maxTimeControl, buf);
}

/// @brief         Loads the default menu settings from the controls into the internal variables
/// @param p_param The configuration xml file handle
static void LoadDefaultSettings()
{
    m_task = GfuiRadioButtonListGetSelected(s_scrHandle, m_taskControl);

    m_indicators.Audio = GfuiCheckboxIsChecked(s_scrHandle, m_indicatorsControl[0]);
    m_indicators.Icon  = GfuiCheckboxIsChecked(s_scrHandle, m_indicatorsControl[1]);
    m_indicators.Text  = GfuiCheckboxIsChecked(s_scrHandle, m_indicatorsControl[2]);

    m_interventionType = GfuiRadioButtonListGetSelected(s_scrHandle, m_interventionTypeControl);

    m_pControl.ControlGas = GfuiCheckboxIsChecked(s_scrHandle, m_pControlControl[0]);
    m_pControl.ControlInterventionToggle = GfuiCheckboxIsChecked(s_scrHandle, m_pControlControl[1]);
    m_pControl.ControlSteering = GfuiCheckboxIsChecked(s_scrHandle, m_pControlControl[2]);
    m_pControl.ForceFeedback = GfuiCheckboxIsChecked(s_scrHandle, m_pControlControl[3]);

    m_maxTime = std::stoi(GfuiEditboxGetString(s_scrHandle, m_maxTimeControl));
}

/// @brief        Loads the settings from the config file into the internal variables
/// @param p_param The configuration xml file handle
static void LoadConfigSettings(void* p_param)
{
    // Retrieve all setting variables from the xml file and assigning them to the internal variables
    m_task = std::stoi(GfParmGetStr(p_param, PRM_TASKS, GFMNU_ATTR_SELECTED, NULL));

    m_indicators.Audio = GfuiMenuControlGetBoolean(p_param, PRM_INDCTR_AUDITORY, GFMNU_ATTR_CHECKED, NULL);
    m_indicators.Icon  = GfuiMenuControlGetBoolean(p_param, PRM_INDCTR_VISUAL,   GFMNU_ATTR_CHECKED, NULL);
    m_indicators.Text  = GfuiMenuControlGetBoolean(p_param, PRM_INDCTR_TEXT,     GFMNU_ATTR_CHECKED, NULL);

    m_interventionType = std::stoi(GfParmGetStr(p_param, PRM_INTERVENTIONTYPE, GFMNU_ATTR_SELECTED, NULL));

    m_pControl.ControlGas                = GfuiMenuControlGetBoolean(p_param, PRM_CTRL_GAS,         GFMNU_ATTR_CHECKED, NULL);
    m_pControl.ControlInterventionToggle = GfuiMenuControlGetBoolean(p_param, PRM_CTRL_INTRV_TGGLE, GFMNU_ATTR_CHECKED, NULL);
    m_pControl.ControlSteering           = GfuiMenuControlGetBoolean(p_param, PRM_CTRL_STEERING,    GFMNU_ATTR_CHECKED, NULL);

    m_pControl.ForceFeedback             = GfuiMenuControlGetBoolean(p_param, PRM_FORCE_FEEDBACK,   GFMNU_ATTR_CHECKED, NULL);
    m_pControl.RecordSession             = GfuiMenuControlGetBoolean(p_param, PRM_RECORD_TGGLE,     GFMNU_ATTR_CHECKED, NULL);
    m_pControl.BBRecordSession           = GfuiMenuControlGetBoolean(p_param, PRM_RECORD_BB_TGGLE,  GFMNU_ATTR_CHECKED, NULL);

    // Set the max time setting from the xml file
    m_maxTime = std::stoi(GfParmGetStr(p_param, PRM_MAX_TIME, GFMNU_ATTR_TEXT, NULL));

    // Match the menu buttons with the initialized values / checking checkboxes and radiobuttons
    SynchronizeControls();
}

/// @brief Loads the user menu settings from the local config file
static void OnActivate(void* /* dummy */)
{
    // Retrieves the saved user xml file, if it doesn't exist the settings are already initialized in ResearcherMenuInit
    std::string strPath("config/ResearcherMenu.xml");
    char buf[512];
    sprintf(buf, "%s%s", GfLocalDir(), strPath.c_str());
    if (GfFileExists(buf)) 
    {
        void* param = GfParmReadFile(buf, GFPARM_RMODE_STD);
        // Initialize settings with the retrieved xml file
        LoadConfigSettings(param);
        return;
    }
    LoadDefaultSettings();
}

/// @brief            Initializes the researcher menu
/// @param p_nextMenu The scrHandle of the next menu
/// @return           The researcherMenu scrHandle
void* ResearcherMenuInit(void* p_nextMenu)
{
    // Return if screen already created
    if (s_scrHandle) return s_scrHandle;
    
    // Otherwise, create the screen
    s_scrHandle = GfuiScreenCreate((float*)NULL, NULL, OnActivate,
                                   NULL, (tfuiCallback)NULL, 1);
    s_nextHandle = p_nextMenu;

    void* param = GfuiMenuLoad("ResearcherMenu.xml");
    GfuiMenuCreateStaticControls(s_scrHandle, param);

    // Task radio button controls
    m_taskControl = GfuiMenuCreateRadioButtonListControl(s_scrHandle, param, PRM_TASKS, NULL, SelectTask);

    // Indicator checkboxes controls
    m_indicatorsControl[0] = GfuiMenuCreateCheckboxControl(s_scrHandle, param, PRM_INDCTR_AUDITORY, NULL, SelectAudio);
    m_indicatorsControl[1] = GfuiMenuCreateCheckboxControl(s_scrHandle, param, PRM_INDCTR_VISUAL,   NULL, SelectIcon);
    m_indicatorsControl[2] = GfuiMenuCreateCheckboxControl(s_scrHandle, param, PRM_INDCTR_TEXT,     NULL, SelectText);

    // InterventionTypes radio button controls
    m_interventionTypeControl = GfuiMenuCreateRadioButtonListControl(s_scrHandle, param, PRM_INTERVENTIONTYPE, NULL, SelectInterventionType);

    // Environment checkboxes controls
    GfuiMenuCreateRadioButtonListControl(s_scrHandle, param, PRM_ENVIRONMENT, NULL, SelectEnvironment);

    // Participant-Control checkboxes controls
    m_pControlControl[0] = GfuiMenuCreateCheckboxControl(s_scrHandle, param, PRM_CTRL_GAS,         NULL, SelectControlGas);
    m_pControlControl[1] = GfuiMenuCreateCheckboxControl(s_scrHandle, param, PRM_CTRL_INTRV_TGGLE, NULL, SelectControlInterventionOnOff);
    m_pControlControl[2] = GfuiMenuCreateCheckboxControl(s_scrHandle, param, PRM_CTRL_STEERING,    NULL, SelectControlSteering);
  
    // Other options checkbox controls
    m_pControlControl[3] = GfuiMenuCreateCheckboxControl(s_scrHandle, param, PRM_FORCE_FEEDBACK,   NULL, SelectForceFeedback);
    m_pControlControl[4] = GfuiMenuCreateCheckboxControl(s_scrHandle, param, PRM_RECORD_TGGLE,     NULL, SelectRecorderOnOff);
    m_pControlControl[5] = GfuiMenuCreateCheckboxControl(s_scrHandle, param, PRM_RECORD_BB_TGGLE,  NULL, SelectBBRecorderOnOff);

    // Textbox controls
    m_maxTimeControl = GfuiMenuCreateEditControl(s_scrHandle, param, PRM_MAX_TIME, NULL, NULL, SetMaxTime);
    m_userIdControl  = GfuiMenuCreateEditControl(s_scrHandle, param, PRM_USER_ID,  NULL, NULL, SetUserId);

    // ApplyButton control
    GfuiMenuCreateButtonControl(s_scrHandle, param, "ApplyButton", s_scrHandle, SaveSettings);

    GfParmReleaseHandle(param);

    // Keyboard button controls
    GfuiAddKey(s_scrHandle, GFUIK_RETURN, "Apply", NULL, SaveSettings, NULL);
    GfuiAddKey(s_scrHandle, GFUIK_F1, "Help", s_scrHandle, GfuiHelpScreen, NULL);
    GfuiAddKey(s_scrHandle, GFUIK_F12, "Screen-Shot", NULL, GfuiScreenShot, NULL);

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
