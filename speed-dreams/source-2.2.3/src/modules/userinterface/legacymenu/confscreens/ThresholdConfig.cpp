#include <tgfclient.h>
#include <iostream>
#include "ThresholdConfig.h"
#include "../legacymenu.h"

#define SND_PARAM_FILE    "config/Threshhold.xml"
#define SND_SCT_THRESHOLD "Threshold Settings"
#define SND_ATT_ACCEL     "Accel"
#define SND_ATT_BRAKE     "Brake"
#define SND_ATT_STEER     "Steer"

static void* s_scrHandle = nullptr;
static void* s_prevHandle = nullptr;

// Data to store
float m_accelVal = 0.0f;
float m_brakeVal = 0.9f;
float m_steerVal = 0.04f;

// GUI settings Id's
int m_accelThresholdControl;
int m_brakeThresholdControl;
int m_steerThresholdControl;

/// @brief Handle input in the accel threshold textbox
static void SetAccelThreshold(void*)
{
    // Get accel threshold from text box, clamped between 0 and 1
    char* val = GfuiEditboxGetString(s_scrHandle, m_accelThresholdControl);
    char* endptr;
    m_accelVal = (float)strtol(val, &endptr, 0);
    if (*endptr != '\0')
        std::cerr << "Could not convert " << val << " to long and leftover string is: " << endptr << std::endl;

    if (m_accelVal > 1.0f)
        m_accelVal = 1.0f;
    else if (m_accelVal < 0.0f)
        m_accelVal = 0.0f;

    // Write the new (clamped) value to the text box.
    char buf[32];
    sprintf(buf, "%g", m_accelVal);
    GfuiEditboxSetString(s_scrHandle, m_accelThresholdControl, buf);
}

/// @brief Handle input in the brake threshold textbox
static void SetBrakeThreshold(void*)
{
    // Get accel threshold from text box, clamped between 0 and 1
    char* val = GfuiEditboxGetString(s_scrHandle, m_brakeThresholdControl);
    char* endptr;
    m_brakeVal = (float)strtol(val, &endptr, 0);
    if (*endptr != '\0')
        std::cerr << "Could not convert " << val << " to long and leftover string is: " << endptr << std::endl;

    if (m_brakeVal > 1.0f)
        m_brakeVal = 1.0f;
    else if (m_brakeVal < 0.0f)
        m_brakeVal = 0.0f;

    // Write the new (clamped) value to the text box.
    char buf[32];
    sprintf(buf, "%g", m_brakeVal);
    GfuiEditboxSetString(s_scrHandle, m_brakeThresholdControl, buf);
}

/// @brief Handle input in the steer threshold textbox
static void SetSteerThreshold(void*)
{
    // Get accel threshold from text box, clamped between 0 and 1
    char* val = GfuiEditboxGetString(s_scrHandle, m_steerThresholdControl);
    char* endptr;
    m_steerVal = (float)strtol(val, &endptr, 0);
    if (*endptr != '\0')
        std::cerr << "Could not convert " << val << " to long and leftover string is: " << endptr << std::endl;


    if (m_steerVal > 1.0f)
        m_steerVal = 1.0f;
    else if (m_steerVal < 0.0f)
        m_steerVal = 0.0f;

    // Write the new (clamped) value to the text box.
    char buf[32];
    sprintf(buf, "%g", m_steerVal);
    GfuiEditboxSetString(s_scrHandle, m_steerThresholdControl, buf);
}

/// @brief Read threshold config from local xml file
static void ReadThresholdConfig()
{
    char buf[1024];

    // Threshold interface.
    sprintf(buf, "%s%s", GfLocalDir(), SND_PARAM_FILE);
    void* paramHandle = GfParmReadFile(buf, GFPARM_RMODE_REREAD | GFPARM_RMODE_CREAT);

    // Accel threshold
    m_accelVal = GfParmGetNum(paramHandle, SND_SCT_THRESHOLD, SND_ATT_ACCEL, "%", 0.0f);
    if (m_accelVal > 1.0f)
    {
        m_accelVal = 1.0f;
    }
    else if (m_accelVal < 0.0f)
    {
        m_accelVal = 0.0f;
    }
    sprintf(buf, "%g", m_accelVal);
    GfuiEditboxSetString(s_scrHandle, m_accelThresholdControl, buf);

    // Brake threshold
    m_brakeVal = GfParmGetNum(paramHandle, SND_SCT_THRESHOLD, SND_ATT_BRAKE, "%", 0.0f);
    if (m_brakeVal > 1.0f)
    {
        m_brakeVal = 1.0f;
    }
    else if (m_brakeVal < 0.0f)
    {
        m_brakeVal = 0.0f;
    }
    sprintf(buf, "%g", m_brakeVal);
    GfuiEditboxSetString(s_scrHandle, m_brakeThresholdControl, buf);

    // Steer threshold
    m_steerVal = GfParmGetNum(paramHandle, SND_SCT_THRESHOLD, SND_ATT_STEER, "%", 0.0f);
    if (m_steerVal > 1.0f)
    {
        m_steerVal = 1.0f;
    }
    else if (m_steerVal < 0.0f)
    {
        m_steerVal = 0.0f;
    }
    sprintf(buf, "%g", m_steerVal);
    GfuiEditboxSetString(s_scrHandle, m_steerThresholdControl, buf);

    GfParmReleaseHandle(paramHandle);
}

/// @brief Save the chosen values in the corresponding parameter file.
static void SaveThresholdOptions(void*)
{
    // Force current edit to lose focus (if one has it) and update associated variable.
    GfuiUnSelectCurrent();

    char buf[1024];
    sprintf(buf, "%s%s", GfLocalDir(), SND_PARAM_FILE);
    void* paramHandle = GfParmReadFile(buf, GFPARM_RMODE_REREAD | GFPARM_RMODE_CREAT);
    GfParmSetNum(paramHandle, SND_SCT_THRESHOLD, SND_ATT_ACCEL, "%", m_accelVal);
    GfParmSetNum(paramHandle, SND_SCT_THRESHOLD, SND_ATT_BRAKE, "%", m_brakeVal);
    GfParmSetNum(paramHandle, SND_SCT_THRESHOLD, SND_ATT_STEER, "%", m_steerVal);

    GfParmWriteFile(nullptr, paramHandle, "Threshhold");
    GfParmReleaseHandle(paramHandle);

    // Return to previous screen.
    GfuiScreenActivate(s_prevHandle);
}

/// @brief Loads the user menu settings from the local config file
static void OnActivate(void* /* dummy */)
{
    ReadThresholdConfig();
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
