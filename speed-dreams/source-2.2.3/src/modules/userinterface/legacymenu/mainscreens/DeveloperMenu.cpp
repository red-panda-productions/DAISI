/*
 * This program has been developed by students from the bachelor Computer Science at
 * Utrecht University within the Software Project course.
 * © Copyright Utrecht University (Department of Information and Computing Sciences)
 */

#include <tgfclient.h>
#include "legacymenu.h"
#include "DeveloperMenu.h"
#include "ConfigEnums.h"
#include "Mediator.h"
#include "guimenu.h"
#include "FileDialogManager.h"
#include "RppUtils.hpp"
#include "FileSystem.hpp"
#include "racemanagers.h"
#include "IndicatorConfig.h"

// Parameters used in the xml files
#define PRM_SYNC               "SynchronizationButtonList"
#define PRM_RECORD_TOGGLE      "CheckboxRecorderToggle"
#define PRM_CHOOSE_REPLAY      "ChooseReplayFileButton"
#define PRM_START_REPLAY       "StartReplayButton"
#define PRM_DECISION_THRESHOLD "Threshold Settings"
#define GFMNU_ATTR_PATH        "path"
#define GFMNU_ATT_ACCEL        "Accel"
#define GFMNU_ATT_BRAKE        "Brake"
#define GFMNU_ATT_STEER        "Steer"

#define DEV_FILEPATH    "config/DeveloperMenu.xml"
#define DEV_XMLPATH     "data/menu/DeveloperMenu.xml"
#define DEV_SCREEN_NAME "DeveloperMenu"

#define MSG_CHOOSE_REPLAY_NORMAL_TEXT "Choose Replay File: "

#define RACE_MANAGER_NAME "replay_visual"

#ifdef WIN32
#define INTEGRATION_TESTS_EXTENSION ".exe"
#else
#define INTEGRATION_TESTS_EXTENSION
#endif

#define INTEGRATION_TESTS_BLACK_BOX                   "replayBlackbox/SDAReplay" INTEGRATION_TESTS_EXTENSION
#define INTEGRATION_TESTS_BLACK_BOX_WORKING_DIRECTORY "replayBlackbox/"

static void* s_scrHandle = nullptr;
static void* s_prevHandle = nullptr;

// Control for synchronization option
int m_syncButtonList;

// Control for replay recorder
int m_replayRecorder;

// Control for choosing replay file
int m_chooseReplayFileButton;

// Control for starting a replay
int m_startReplayButton;

// Controls for decision thresholds
int m_accelThresholdControl;
int m_brakeThresholdControl;
int m_steerThresholdControl;

// The current interventionType set in the researcher menu
InterventionType m_tempInterventionType;

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
}

/// @brief Makes sure all visuals display the internal values
static void SynchronizeControls()
{
    GfuiRadioButtonListSetSelected(s_scrHandle, m_syncButtonList, (int)m_sync);

    GfuiCheckboxSetChecked(s_scrHandle, m_replayRecorder, m_replayRecorderOn);

    if (m_replayFileChosen)
    {
        filesystem::path path = m_replayFilePath;
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
        GfParmReleaseHandle(param);
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
    SynchronizeControls();
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
    bool success = SelectFile(buf, err, true,nullptr,  nullptr, 0, true);
    if (!success)
    {
        return;
    }

    // Visual feedback of choice
    filesystem::path path = buf;
    std::string buttonText = MSG_CHOOSE_REPLAY_NORMAL_TEXT + path.filename().string();
    GfuiButtonSetText(s_scrHandle, m_chooseReplayFileButton, buttonText.c_str());

    // Copy into actual variable
    strcpy_s(m_replayFilePath, BLACKBOX_PATH_SIZE, buf);
    m_replayFileChosen = true;
}

/// @brief                    Write the thresholdvalue in the textbox
/// @param p_threshold        The value to write in the textbox
/// @param p_thresholdControl The edit box to write to
static void WriteThresholdValue(float& p_threshold, int p_thresholdControl)
{
    char buf[32];
    sprintf(buf, "%g", p_threshold);
    GfuiEditboxSetString(s_scrHandle, p_thresholdControl, buf);
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
    WriteThresholdValue(p_threshold, p_thresholdControl);
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

/// @brief Starts the raceEngine according to legacymenu.cpp::startRace and sets replay values
static void StartReplay(void*)
{
    if (!Recorder::LoadRecording(m_replayFilePath))
    {
        GfLogError("Failed to read recording: {}\n", m_replayFilePath);
        GfuiScreenActivate(DeveloperMenuInit(s_scrHandle));
        return;
    }

    // get the racemanager and run it if there's such a race manager.
    GfRaceManager* RaceManager = GfRaceManagers::self()->getRaceManager(RACE_MANAGER_NAME);
    if (RaceManager)
    {
        // Initialize the race engine.
        LmRaceEngine().reset();

        // Give the selected race manager to the race engine.
        LmRaceEngine().selectRaceman(RaceManager);

        // Configure the new race (but don't enter the config. menu tree).
        LmRaceEngine().configureRace(/* bInteractive */ false);

        std::string bbArgs = GenerateBBArguments(filesystem::path(m_replayFilePath).append(DECISIONS_RECORDING_FILE_NAME), INTEGRATION_TESTS_BLACK_BOX);

        // Start the replay black box
        StartExecutable(INTEGRATION_TESTS_BLACK_BOX, bbArgs.c_str());

        // Start the race engine state automaton
        LmRaceEngine().startNewRace();
        return;
    }
    else
    {
        GfLogError("No such race type '%s'\n", RACE_MANAGER_NAME);
        GfuiScreenActivate(DeveloperMenuInit(s_scrHandle));
    }
}

/// @brief Set the default values of threshold boxes based on the InterventionType.
static void SetDefaultThresholdValues(void*)
{
    // load the xmlhandle
    char buf[512];
    sprintf(buf, "%s%s", GfDataDir(), DEV_XMLPATH);
    void* xmlHandle = GfParmReadFile(buf, GFPARM_RMODE_STD);
    if (xmlHandle == nullptr)
        throw std::invalid_argument("DeveloperMenu.xml does not exists");

    // load the path in the xml based on the intervention type
    std::string m_path = "DefaultThresholdValues/";

    // if the intervention type is no signals, don't change any values
    if (m_tempInterventionType == INTERVENTION_TYPE_NO_SIGNALS)
        return;
    m_path += s_interventionTypeString[m_tempInterventionType];

    // set the values to their default determined by the xml file.
    m_decisionThresholds.Accel = GfParmGetNum(xmlHandle, m_path.c_str(), GFMNU_ATT_ACCEL, nullptr, 0.9f);
    m_decisionThresholds.Brake = GfParmGetNum(xmlHandle, m_path.c_str(), GFMNU_ATT_BRAKE, nullptr, 0.9f);
    m_decisionThresholds.Steer = GfParmGetNum(xmlHandle, m_path.c_str(), GFMNU_ATT_STEER, nullptr, 0.05f);

    // set the edit boxes to the correct value
    WriteThresholdValue(m_decisionThresholds.Accel, m_accelThresholdControl);
    WriteThresholdValue(m_decisionThresholds.Brake, m_brakeThresholdControl);
    WriteThresholdValue(m_decisionThresholds.Steer, m_steerThresholdControl);

    GfParmReleaseHandle(xmlHandle);
}

/// @brief Set the default values of threshold boxes based on the InterventionType.
void RemoteSetDefaultThresholdValues()
{
    SetDefaultThresholdValues(nullptr);
    SaveSettings();
}

/// @brief                    Set the temporary interventionType to p_interventionType
/// @param p_interventionType The intervention type
void SetTempInterventionType(InterventionType p_interventionType)
{
    m_tempInterventionType = p_interventionType;
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

    // Apply and cancel buttons
    GfuiMenuCreateButtonControl(s_scrHandle, param, "CancelButton", s_scrHandle, SwitchToResearcherMenu);
    GfuiMenuCreateButtonControl(s_scrHandle, param, "ApplyButton", s_scrHandle, SaveAndGoBack);

    // BB sync option
    m_syncButtonList = GfuiMenuCreateRadioButtonListControl(s_scrHandle, param, PRM_SYNC, nullptr, SelectSync);

    // Replay options
    m_replayRecorder = GfuiMenuCreateCheckboxControl(s_scrHandle, param, PRM_RECORD_TOGGLE, nullptr, SelectRecorderOnOff);
    m_chooseReplayFileButton = GfuiMenuCreateButtonControl(s_scrHandle, param, PRM_CHOOSE_REPLAY, s_scrHandle, ChooseReplayFile);
    m_startReplayButton = GfuiMenuCreateButtonControl(s_scrHandle, param, PRM_START_REPLAY, s_scrHandle, StartReplay);

    // Decision threshold options
    m_accelThresholdControl = GfuiMenuCreateEditControl(s_scrHandle, param, "AccelThresholdEdit", nullptr, nullptr, SetAccelThreshold);
    m_brakeThresholdControl = GfuiMenuCreateEditControl(s_scrHandle, param, "BrakeThresholdEdit", nullptr, nullptr, SetBrakeThreshold);
    m_steerThresholdControl = GfuiMenuCreateEditControl(s_scrHandle, param, "SteerThresholdEdit", nullptr, nullptr, SetSteerThreshold);
    GfuiMenuCreateButtonControl(s_scrHandle, param, "DefaultButton", nullptr, SetDefaultThresholdValues);

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