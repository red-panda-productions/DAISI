#include <tgfclient.h>
#include "legacymenu.h"
#include "DeveloperMenu.h"
#include "ConfigEnums.h"
#include "Mediator.h"
#include "guimenu.h"
#include "../rppUtils/FileDialog.hpp"
#include "../rppUtils/RppUtils.hpp"
#include <experimental/filesystem>

// Parameters used in the xml files
#define PRM_SYNC               "SynchronizationButtonList"
#define PRM_RECORD_TOGGLE      "CheckboxRecorderToggle"
#define PRM_CHOOSE_REPLAY      "ChooseReplayFileButton"
#define PRM_DECISION_THRESHOLD "Threshold Settings"
#define GFMNU_ATTR_PATH        "path"
#define GFMNU_ATT_ACCEL        "Accel"
#define GFMNU_ATT_BRAKE        "Brake"
#define GFMNU_ATT_STEER        "Steer"

#define DEV_FILEPATH    "config/DeveloperMenu.xml"
#define DEV_SCREEN_NAME "DeveloperMenu"

#define MSG_CHOOSE_REPLAY_NORMAL_TEXT "Choose Replay File: "

static void* s_scrHandle = nullptr;
static void* s_prevHandle = nullptr;

// Control for apply button
int m_applyButtonDev;

// Control for synchronization option
int m_syncButtonList;

// Control for replay recorder
int m_replayRecorder;

// Control for choosing replay file
int m_chooseReplayFileButton;

// Controls for decision thresholds
int m_accelThresholdControl;
int m_brakeThresholdControl;
int m_steerThresholdControl;

// Synchronization type
SyncType m_sync;

// Recorder status
bool m_replayRecorderOn;

// Decision threshold values
tDecisionThresholds m_decisionThresholds = {STANDARD_THRESHOLD_ACCEL, STANDARD_THRESHOLD_BRAKE, STANDARD_THRESHOLD_STEER};

// Replay file
char m_replayFilePath[MAX_PATH_SIZE];
bool m_replayFileChosen = false;

/// @brief         Loads the settings from the config file
/// @param p_param A handle to the parameter file
static void LoadSettingsFromFile(void* p_param)
{
    m_sync = std::stoi(GfParmGetStr(p_param, PRM_SYNC, GFMNU_ATTR_SELECTED, "1"));

    m_replayRecorderOn = GfuiMenuControlGetBoolean(p_param, PRM_RECORD_TOGGLE, GFMNU_ATTR_CHECKED, false);

    const char* filePath = GfParmGetStr(p_param, PRM_CHOOSE_REPLAY, GFMNU_ATTR_PATH, nullptr);
    if (filePath)
    {
        strcpy_s(m_replayFilePath, MAX_PATH_SIZE, filePath);
        m_replayFileChosen = true;
    }

    // Get threshold values
    m_decisionThresholds.Accel = GfParmGetNum(p_param, PRM_DECISION_THRESHOLD, GFMNU_ATT_ACCEL, "%", STANDARD_THRESHOLD_ACCEL);
    m_decisionThresholds.Brake = GfParmGetNum(p_param, PRM_DECISION_THRESHOLD, GFMNU_ATT_BRAKE, "%", STANDARD_THRESHOLD_BRAKE);
    m_decisionThresholds.Steer = GfParmGetNum(p_param, PRM_DECISION_THRESHOLD, GFMNU_ATT_STEER, "%", STANDARD_THRESHOLD_STEER);

    // Clamp threshold values
    Clamp(m_decisionThresholds.Accel, 0.0f, 1.0f);
    Clamp(m_decisionThresholds.Brake, 0.0f, 1.0f);
    Clamp(m_decisionThresholds.Steer, 0.0f, 1.0f);

    GfParmReleaseHandle(p_param);
}

/// @brief Makes sure all visuals display the internal values
static void SynchronizeControls()
{
    GfuiRadioButtonListSetSelected(s_scrHandle, m_syncButtonList, (int)m_sync);

    GfuiCheckboxSetChecked(s_scrHandle, m_replayRecorder, m_replayRecorderOn);

    if (m_replayFileChosen)
    {
        std::experimental::filesystem::path path = m_replayFilePath;
        std::string buttonText = MSG_CHOOSE_REPLAY_NORMAL_TEXT + path.filename().string();
        GfuiButtonSetText(s_scrHandle, m_chooseReplayFileButton, buttonText.c_str());
    }

    char buf[1024];
    GfuiEditboxSetString(s_scrHandle, m_accelThresholdControl, FloatToCharArr(m_decisionThresholds.Accel, buf));
    GfuiEditboxSetString(s_scrHandle, m_brakeThresholdControl, FloatToCharArr(m_decisionThresholds.Brake, buf));
    GfuiEditboxSetString(s_scrHandle, m_steerThresholdControl, FloatToCharArr(m_decisionThresholds.Steer, buf));
}

/// @brief Loads default settings
static void LoadDefaultSettings()
{
    m_sync = GfuiRadioButtonListGetSelected(s_scrHandle, m_syncButtonList);

    m_replayRecorderOn = GfuiCheckboxIsChecked(s_scrHandle, m_replayRecorder);
}

/// @brief Loads (if possible) the settings; otherwise, the control's default settings will be used
static void LoadSettings()
{
    char buf[512];
    sprintf(buf, "%s%s", GfLocalDir(), DEV_FILEPATH);
    if (GfFileExists(buf))
    {
        void* param = GfParmReadFile(buf, GFPARM_RMODE_STD);
        // Initialize settings with the retrieved xml file
        LoadSettingsFromFile(param);
        SynchronizeControls();
        return;
    }

    LoadDefaultSettings();
}

/// @brief Saves the settings to a file
static void SaveSettingsToFile()
{
    std::string dstStr(DEV_FILEPATH);
    char dst[512];
    sprintf(dst, "%s%s", GfLocalDir(), dstStr.c_str());
    void* readParam = GfParmReadFile(dst, GFPARM_RMODE_REREAD | GFPARM_RMODE_CREAT);

    // Force current edit to lose focus (if one has it) and update associated variable.
    GfuiUnSelectCurrent();

    // Write sync type
    char val[32];
    sprintf(val, "%d", m_sync);
    GfParmSetStr(readParam, PRM_SYNC, GFMNU_ATTR_SELECTED, val);

    // Write recorder status
    GfParmSetStr(readParam, PRM_RECORD_TOGGLE, GFMNU_ATTR_CHECKED, GfuiMenuBoolToStr(m_replayRecorderOn));

    // Write replay file path
    GfParmSetStr(readParam, PRM_CHOOSE_REPLAY, GFMNU_ATTR_PATH, m_replayFilePath);

    // Write decision thresholds
    GfParmSetNum(readParam, PRM_DECISION_THRESHOLD, GFMNU_ATT_ACCEL, "%", m_decisionThresholds.Accel);
    GfParmSetNum(readParam, PRM_DECISION_THRESHOLD, GFMNU_ATT_BRAKE, "%", m_decisionThresholds.Brake);
    GfParmSetNum(readParam, PRM_DECISION_THRESHOLD, GFMNU_ATT_STEER, "%", m_decisionThresholds.Steer);

    // Write queued changes
    GfParmWriteFile(nullptr, readParam, DEV_SCREEN_NAME);

    GfParmReleaseHandle(readParam);
}

/// @brief Saves the settings so the mediator (or future instances) can access them
static void SaveSettings()
{
    SMediator* mediator = SMediator::GetInstance();

    mediator->SetBlackBoxSyncOption(m_sync == 1);
    mediator->SetReplayRecorderSetting(m_replayRecorderOn);
    mediator->SetReplayFolder(m_replayFilePath);
    mediator->SetThresholdSettings(m_decisionThresholds);

    SaveSettingsToFile();
}

/// @brief Takes actions that need to be run on activation of the developer screen
static void OnActivate(void* /* dummy */)
{
    LoadSettings();
}

/// @brief Switches back to the reseacher menu
static void SwitchToResearcherMenu(void* /* dummy */)
{
    // go back to the researcher screen
    GfuiScreenActivate(s_prevHandle);
}

/// @brief Saves the settings and then switched back to the researcher menu.
static void SaveAndGoBack(void* /* dummy */)
{
    SaveSettings();

    SwitchToResearcherMenu(nullptr);
}

/// @brief        Sets the type of synchronization
/// @param p_info Information about the radio button list
static void SelectSync(tRadioButtonInfo* p_info)
{
    m_sync = (SyncType)p_info->Selected;
}

/// @brief        Enables/disables the replay recorder
/// @param p_info Information on the checkbox
static void SelectRecorderOnOff(tCheckBoxInfo* p_info)
{
    m_replayRecorderOn = p_info->bChecked;
}

/// @brief Chooses the replay file
static void ChooseReplayFile(void* /* dummy */)
{
    char buf[MAX_PATH_SIZE];
    char err[MAX_PATH_SIZE];
    bool success = SelectFile(buf, err, true);
    if (!success)
    {
        return;
    }

    // Visual feedback of choice
    std::experimental::filesystem::path path = buf;
    std::string buttonText = MSG_CHOOSE_REPLAY_NORMAL_TEXT + path.filename().string();
    GfuiButtonSetText(s_scrHandle, m_chooseReplayFileButton, buttonText.c_str());

    // Copy into actual variable
    strcpy_s(m_replayFilePath, BLACKBOX_PATH_SIZE, buf);
    m_replayFileChosen = true;
}

/// @brief                    Read the edit box value and clamp it
/// @param p_threshold        The value to change
/// @param p_thresholdControl The edit box to read from and write to
static void SetThreshold(float& p_threshold, int p_thresholdControl)
{
    // Get threshold from text box, clamp it between 0 and 1
    p_threshold = CharArrToFloat(GfuiEditboxGetString(s_scrHandle, p_thresholdControl));
    Clamp(p_threshold, 0.0f, 1.0f);

    // Write the clamped value to the text box.
    char buf[32];
    sprintf(buf, "%g", p_threshold);
    GfuiEditboxSetString(s_scrHandle, p_thresholdControl, buf);
}

/// @brief Handle input in the accel threshold textbox
static void SetAccelThreshold(void*)
{
    SetThreshold(m_decisionThresholds.Accel, m_accelThresholdControl);
}

/// @brief Handle input in the brake threshold textbox
static void SetBrakeThreshold(void*)
{
    SetThreshold(m_decisionThresholds.Brake, m_brakeThresholdControl);
}

/// @brief Handle input in the steer threshold textbox
static void SetSteerThreshold(void*)
{
    SetThreshold(m_decisionThresholds.Steer, m_steerThresholdControl);
}

/// @brief            Initializes the developer menu
/// @param p_prevMenu A handle to the previous menu
/// @returns          A handle to the developer menu
void* DeveloperMenuInit(void* p_prevMenu)
{
    // screen already created
    if (s_scrHandle) return s_scrHandle;

    s_scrHandle = GfuiScreenCreate((float*)nullptr, nullptr, OnActivate, nullptr,
                                   (tfuiCallback) nullptr, 1);
    s_prevHandle = p_prevMenu;

    void* param = GfuiMenuLoad("DeveloperMenu.xml");
    GfuiMenuCreateStaticControls(s_scrHandle, param);

    // Buttons
    GfuiMenuCreateButtonControl(s_scrHandle, param, "CancelButton", s_scrHandle, SwitchToResearcherMenu);
    m_applyButtonDev = GfuiMenuCreateButtonControl(s_scrHandle, param, "ApplyButton", s_scrHandle, SaveAndGoBack);
    m_syncButtonList = GfuiMenuCreateRadioButtonListControl(s_scrHandle, param, PRM_SYNC, nullptr, SelectSync);
    m_replayRecorder = GfuiMenuCreateCheckboxControl(s_scrHandle, param, PRM_RECORD_TOGGLE, nullptr, SelectRecorderOnOff);
    m_chooseReplayFileButton = GfuiMenuCreateButtonControl(s_scrHandle, param, PRM_CHOOSE_REPLAY, s_scrHandle, ChooseReplayFile);

    // Edit boxes
    m_accelThresholdControl = GfuiMenuCreateEditControl(s_scrHandle, param, "AccelThresholdEdit", nullptr, nullptr, SetAccelThreshold);
    m_brakeThresholdControl = GfuiMenuCreateEditControl(s_scrHandle, param, "BrakeThresholdEdit", nullptr, nullptr, SetBrakeThreshold);
    m_steerThresholdControl = GfuiMenuCreateEditControl(s_scrHandle, param, "SteerThresholdEdit", nullptr, nullptr, SetSteerThreshold);

    GfParmReleaseHandle(param);

    // Keyboard button controls
    GfuiAddKey(s_scrHandle, GFUIK_RETURN, "Apply", nullptr, SaveAndGoBack, nullptr);
    GfuiAddKey(s_scrHandle, GFUIK_ESCAPE, "Cancel", s_prevHandle, SwitchToResearcherMenu, nullptr);
    GfuiMenuDefaultKeysAdd(s_scrHandle);

    return s_scrHandle;
}

/// @brief Activates the developer menu
void DeveloperMenuRun(void*)
{
    GfuiScreenActivate(s_scrHandle);
}

/// @brief Makes sure these settings are still set in the SDAConfig, even if this menu is never opened and exited via apply,
/// as otherwise there is no guarantee on what the settings are.
void ConfigureDeveloperSettings()
{
    LoadSettings();
    SaveSettings();
}