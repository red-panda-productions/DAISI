#include <tgfclient.h>
#include <iostream>
#include "ThresholdConfigMenu.h"
#include "ThresholdConfig.h"
#include "../legacymenu.h"

static void* s_scrHandle = nullptr;
static void* s_prevHandle = nullptr;

// Decision threshold values
tDecisionThresholds m_decisionThresholds = {STANDARD_THRESHOLD_ACCEL, STANDARD_THRESHOLD_BRAKE, STANDARD_THRESHOLD_STEER};

// GUI settings Id's
int m_accelThresholdControl;
int m_brakeThresholdControl;
int m_steerThresholdControl;

/// @brief Handle input in the accel threshold textbox
static void SetAccelThreshold(void*)
{
    // Get accel threshold from text box, clamp it between 0 and 1
    m_decisionThresholds.Accel = CharArrToFloat(GfuiEditboxGetString(s_scrHandle, m_accelThresholdControl));
    ClampFloat(&m_decisionThresholds.Accel, 0, 1);

    // Write the clamped value to the text box.
    char buf[32];
    sprintf(buf, "%g", m_decisionThresholds.Accel);
    GfuiEditboxSetString(s_scrHandle, m_accelThresholdControl, buf);
}

/// @brief Handle input in the brake threshold textbox
static void SetBrakeThreshold(void*)
{
    // Get accel threshold from text box, clamp it between 0 and 1
    m_decisionThresholds.Brake = CharArrToFloat(GfuiEditboxGetString(s_scrHandle, m_brakeThresholdControl));
    ClampFloat(&m_decisionThresholds.Brake, 0, 1);

    // Write the clamped value to the text box.
    char buf[32];
    sprintf(buf, "%g", m_decisionThresholds.Brake);
    GfuiEditboxSetString(s_scrHandle, m_brakeThresholdControl, buf);
}

/// @brief Handle input in the steer threshold textbox
static void SetSteerThreshold(void*)
{
    // Get accel threshold from text box, clamp it between 0 and 1
    m_decisionThresholds.Steer = CharArrToFloat(GfuiEditboxGetString(s_scrHandle, m_steerThresholdControl));
    ClampFloat(&m_decisionThresholds.Steer, 0, 1);

    // Write the clamped value to the text box.
    char buf[32];
    sprintf(buf, "%g", m_decisionThresholds.Steer);
    GfuiEditboxSetString(s_scrHandle, m_steerThresholdControl, buf);
}

/// @brief Synchronizes all the menu controls in the threshold config menu to the internal variables
static void SynchronizeControls()
{
    GfuiEditboxSetString(s_scrHandle, m_accelThresholdControl, FloatToCharArr(m_decisionThresholds.Accel));
    GfuiEditboxSetString(s_scrHandle, m_brakeThresholdControl, FloatToCharArr(m_decisionThresholds.Brake));
    GfuiEditboxSetString(s_scrHandle, m_steerThresholdControl, FloatToCharArr(m_decisionThresholds.Steer));
}

/// @brief Save the chosen values in the corresponding parameter file.
static void SaveThresholdOptions(void*)
{
    // Force current edit to lose focus (if one has it) and update associated variable.
    GfuiUnSelectCurrent();

    char buf[MAX_PATH_SIZE];
    sprintf(buf, "%s%s", GfLocalDir(), PARAM_FILE);
    void* paramHandle = GfParmReadFile(buf, GFPARM_RMODE_REREAD | GFPARM_RMODE_CREAT);
    GfParmSetNum(paramHandle, SCT_THRESHOLD, ATT_ACCEL, "%", m_decisionThresholds.Accel);
    GfParmSetNum(paramHandle, SCT_THRESHOLD, ATT_BRAKE, "%", m_decisionThresholds.Brake);
    GfParmSetNum(paramHandle, SCT_THRESHOLD, ATT_STEER, "%", m_decisionThresholds.Steer);

    GfParmWriteFile(nullptr, paramHandle, "Threshhold");
    GfParmReleaseHandle(paramHandle);

    // Return to previous screen.
    GfuiScreenActivate(s_prevHandle);
}

/// @brief Loads the user menu settings from the local config file
static void OnActivate(void* /* dummy */)
{
    char path[MAX_PATH_SIZE];
    sprintf(path, "%s%s", GfLocalDir(), PARAM_FILE);

    m_decisionThresholds = LoadThresholdSettings(path);
    SynchronizeControls();
}

/// @brief            Initializes the threshold configuration menu
/// @param p_prevMenu The scrHandle of the previous menu
/// @return           The ThresholdConfigMenu scrHandle
void* ThresholdConfigMenuInit(void* p_prevMenu)
{
    // Screen already created
    if (s_scrHandle) return s_scrHandle;

    s_scrHandle = GfuiScreenCreate((float*)nullptr, nullptr, OnActivate,
                                   nullptr, (tfuiCallback) nullptr, 1);
    s_prevHandle = p_prevMenu;

    void* param = GfuiMenuLoad("ThresholdConfigMenu.xml");
    GfuiMenuCreateStaticControls(s_scrHandle, param);

    // ApplyButton and Back-button controls
    GfuiMenuCreateButtonControl(s_scrHandle, param, "ApplyButton", nullptr, SaveThresholdOptions);
    GfuiMenuCreateButtonControl(s_scrHandle, param, "BackButton", s_prevHandle, GfuiScreenActivate);

    // Checkboxes for choosing the simulation information to collect and store in real-time
    m_accelThresholdControl = GfuiMenuCreateEditControl(s_scrHandle, param, "AccelThresholdEdit", nullptr, nullptr, SetAccelThreshold);
    m_brakeThresholdControl = GfuiMenuCreateEditControl(s_scrHandle, param, "BrakeThresholdEdit", nullptr, nullptr, SetBrakeThreshold);
    m_steerThresholdControl = GfuiMenuCreateEditControl(s_scrHandle, param, "SteerThresholdEdit", nullptr, nullptr, SetSteerThreshold);

    GfParmReleaseHandle(param);

    // Keyboard button controls
    GfuiMenuDefaultKeysAdd(s_scrHandle);
    GfuiAddKey(s_scrHandle, GFUIK_RETURN, "Apply", nullptr, SaveThresholdOptions, nullptr);
    GfuiAddKey(s_scrHandle, GFUIK_ESCAPE, "Cancel", s_prevHandle, GfuiScreenActivate, nullptr);

    return s_scrHandle;
}
