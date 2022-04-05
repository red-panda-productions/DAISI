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
tParticipantControl m_pControl = { false, true, true, true };

// Force feedback manager
extern TGFCLIENT_API ForceFeedbackManager forceFeedback;

// Max time
int m_maxTime = 10.0f;
int m_maxTimeControl;

// User ID
char m_userId[32];
int  m_userIdControl;

/// @brief Sets the defaults values
static void OnActivate(void* /* dummy */) { }

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
static void SaveSettingsToDisk() {
    // Open file parameter
    std::string strPath("data/menu/ResearcherMenu.xml");
    char buf[512];
    sprintf(buf, "%s%s", GfDataDir(), strPath.c_str());
    void* readParam = GfParmReadFile(buf, GFPARM_RMODE_REREAD | GFPARM_RMODE_CREAT);

    // Save task settings to xml file
    GfParmSetStr(readParam, "dynamic controls/CheckboxTaskLaneKeeping", "checked", "no");
    GfParmSetStr(readParam, "dynamic controls/CheckboxTaskSpeedControl", "checked", "no");
    switch (m_task)
    {
        case TASK_LANE_KEEPING:
            GfParmSetStr(readParam, "dynamic controls/CheckboxTaskLaneKeeping", "checked", "yes");
            break;
        case TASK_SPEED_CONTROL:
            GfParmSetStr(readParam, "dynamic controls/CheckboxTaskSpeedControl", "checked", "yes");
            break;
    }

    // Save indicator settings to xml file
    const char* audioSetting = m_indicators.Audio ? "yes" : "no";
    GfParmSetStr(readParam, "dynamic controls/CheckboxIndicatorAuditory", "checked", audioSetting);
    const char* iconSetting = m_indicators.Icon ? "yes" : "no";
    GfParmSetStr(readParam, "dynamic controls/CheckboxIndicatorVisual", "checked", iconSetting);
    const char* textSetting = m_indicators.Text ? "yes" : "no";
    GfParmSetStr(readParam, "dynamic controls/CheckboxIndicatorTextual", "checked", textSetting);

    // Save intervention type settings to xml file
    GfParmSetStr(readParam, "dynamic controls/CheckboxTypeNoSignals", "checked", "no");
    GfParmSetStr(readParam, "dynamic controls/CheckboxTypeOnlySignals", "checked", "no");
    GfParmSetStr(readParam, "dynamic controls/CheckboxTypeSharedControl", "checked", "no");
    GfParmSetStr(readParam, "dynamic controls/CheckboxTypeCompleteTakeover", "checked", "no");
    switch (m_interventionType)
    {
        case INTERVENTION_TYPE_NO_SIGNALS:
            GfParmSetStr(readParam, "dynamic controls/CheckboxTypeNoSignals", "checked", "yes");
            break;
        case INTERVENTION_TYPE_ONLY_SIGNALS:
            GfParmSetStr(readParam, "dynamic controls/CheckboxTypeOnlySignals", "checked", "yes");
            break;
        case INTERVENTION_TYPE_SHARED_CONTROL:
            GfParmSetStr(readParam, "dynamic controls/CheckboxTypeSharedControl", "checked", "yes");
            break;
        case INTERVENTION_TYPE_COMPLETE_TAKEOVER:
            GfParmSetStr(readParam, "dynamic controls/CheckboxTypeCompleteTakeover", "checked", "yes");
            break;
    }

    // Save participant control settings to xml file
    const char* controlGas = m_pControl.ControlGas ? "yes" : "no";
    GfParmSetStr(readParam, "dynamic controls/CheckboxPControlGas", "checked", controlGas);
    const char* interventionToggle = m_pControl.ControlInterventionToggle ? "yes" : "no";
    GfParmSetStr(readParam, "dynamic controls/CheckboxPControlInterventionToggle", "checked", interventionToggle);
    const char* controlSteering = m_pControl.ControlSteering ? "yes" : "no";
    GfParmSetStr(readParam, "dynamic controls/CheckboxPControlSteering", "checked", controlSteering);

    // Save max time to xml file
    char buf2[32];
    sprintf(buf2, "%d", m_maxTime);
    GfParmSetStr(readParam, "dynamic controls/MaxTimeEdit", "default value", buf2);

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

/// @brief Sets the task setting in the researcher menu
/// @param p_interventionType boolean array that defines which task should be set
void SetTask(bool* p_tasks) 
{
    if (p_tasks[TASK_LANE_KEEPING]) {
        m_task = TASK_LANE_KEEPING;
        return;
    }
    if (p_tasks[TASK_SPEED_CONTROL]) {
        m_task = TASK_SPEED_CONTROL;
        return;
    }
    m_task = TASK_NO_TASK;
}

/// @brief Sets the intervention setting in the researcher menu
/// @param p_interventionType boolean array that defines which intervention type should be set
void setInterventionType(bool* p_interventionType)
{
    if (p_interventionType[INTERVENTION_TYPE_NO_SIGNALS]) {
        m_interventionType = INTERVENTION_TYPE_NO_SIGNALS;
        return;
    }
    if (p_interventionType[INTERVENTION_TYPE_ONLY_SIGNALS]) {
        m_interventionType = INTERVENTION_TYPE_ONLY_SIGNALS;
        return;
    }
    if(p_interventionType[INTERVENTION_TYPE_SHARED_CONTROL]) {
        m_interventionType = INTERVENTION_TYPE_SHARED_CONTROL;
        return;
    }
    if (p_interventionType[INTERVENTION_TYPE_COMPLETE_TAKEOVER]) {
        m_interventionType = INTERVENTION_TYPE_COMPLETE_TAKEOVER;
        return;
    }
    return;
}

/// @brief         Initializes the menu setting from the ResearcherMenu.xml file
/// @param p_param The configuration menu handle
void InitializeSettings(void* p_param) {
    // Retrieve all setting variables from the xml file
    bool* checkboxTasks = new bool[3];
    checkboxTasks[TASK_LANE_KEEPING] = gfuiMenuGetBoolean(GfParmGetStr(p_param, "dynamic controls/CheckboxTaskLaneKeeping", "checked", NULL), true);
    checkboxTasks[TASK_SPEED_CONTROL] = gfuiMenuGetBoolean(GfParmGetStr(p_param, "dynamic controls/CheckboxTaskSpeedControl", "checked", NULL), false);

    bool checkboxIndicatorAuditory = gfuiMenuGetBoolean(GfParmGetStr(p_param, "dynamic controls/CheckboxIndicatorAuditory", "checked", NULL), true);
    bool checkboxIndicatorVisual = gfuiMenuGetBoolean(GfParmGetStr(p_param, "dynamic controls/CheckboxIndicatorVisual", "checked", NULL), true);
    bool checkboxIndicatorTextual = gfuiMenuGetBoolean(GfParmGetStr(p_param, "dynamic controls/CheckboxIndicatorTextual", "checked", NULL), true);


    bool* checkboxInterventions = new bool[5];
    checkboxInterventions[INTERVENTION_TYPE_NO_SIGNALS] = gfuiMenuGetBoolean(GfParmGetStr(p_param, "dynamic controls/CheckboxTypeNoSignals", "checked", NULL), true);
    checkboxInterventions[INTERVENTION_TYPE_ONLY_SIGNALS] = gfuiMenuGetBoolean(GfParmGetStr(p_param, "dynamic controls/CheckboxTypeOnlySignals", "checked", NULL), false);
    checkboxInterventions[INTERVENTION_TYPE_SHARED_CONTROL] = gfuiMenuGetBoolean(GfParmGetStr(p_param, "dynamic controls/CheckboxTypeSharedControl", "checked", NULL), false);
    checkboxInterventions[INTERVENTION_TYPE_COMPLETE_TAKEOVER] = gfuiMenuGetBoolean(GfParmGetStr(p_param, "dynamic controls/CheckboxTypeCompleteTakeover", "checked", NULL), false);

    bool checkboxInterventionToggle = gfuiMenuGetBoolean(GfParmGetStr(p_param, "dynamic controls/CheckboxPControlInterventionToggle", "checked", NULL), false);
    bool checkboxPControlGas = gfuiMenuGetBoolean(GfParmGetStr(p_param, "dynamic controls/CheckboxPControlGas", "checked", NULL), true);
    bool checkboxPControlSteering = gfuiMenuGetBoolean(GfParmGetStr(p_param, "dynamic controls/CheckboxPControlSteering", "checked", NULL), true);

    // Set the max time setting from the xml file
    m_maxTime = std::stoi(GfParmGetStr(p_param, "dynamic controls/MaxTimeEdit", "default value", NULL));

    // Set the Task settings from the xml file
    setTask(checkboxTasks);

    // Set the indicator settings from the xml file
    m_indicators.Audio = checkboxIndicatorAuditory;
    m_indicators.Icon = checkboxIndicatorVisual;
    m_indicators.Text = checkboxIndicatorTextual;

    // Set the indicator settings from the xml file
    m_pControl.ControlInterventionToggle = checkboxInterventionToggle;
    m_pControl.ControlGas = checkboxPControlGas;
    m_pControl.ControlSteering = checkboxPControlSteering;

    // Set the participant control settings from the xml file 
    setInterventionType(checkboxInterventions);
    
    delete[] checkboxTasks;
    delete[] checkboxInterventions ;
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
    InitializeSettings(param);
    GfuiMenuCreateStaticControls(s_scrHandle, param);

    // Task checkboxes controls
    GfuiMenuCreateCheckboxControl(s_scrHandle, param, "CheckboxTaskLaneKeeping", NULL, SelectLaneKeeping);
    GfuiMenuCreateCheckboxControl(s_scrHandle, param, "CheckboxTaskSpeedControl", NULL, SelectSpeedControl);

    // Indicator checkboxes controls
    GfuiMenuCreateCheckboxControl(s_scrHandle, param, "CheckboxIndicatorAuditory", NULL, SelectAudio);
    GfuiMenuCreateCheckboxControl(s_scrHandle, param, "CheckboxIndicatorVisual", NULL, SelectIcon);
    GfuiMenuCreateCheckboxControl(s_scrHandle, param, "CheckboxIndicatorTextual", NULL, SelectText);

    // InterventionType checkboxes controls
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
    GfuiMenuCreateCheckboxControl(s_scrHandle, param, "CheckboxForceFeedback", NULL, SelectForceFeedback);

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
