#pragma once

#include "ConfigEnums.h"
#include "../simulatedDrivingAssistance/rppUtils/RppUtils.hpp"
#include "../simulatedDrivingAssistance/rppUtils/FileDialog.hpp"

#define THRESHOLD_CONFIG_FILE "config/Threshhold.xml"
#define SCT_THRESHOLD         "Threshold Settings"
#define ATT_ACCEL             "Accel"
#define ATT_BRAKE             "Brake"
#define ATT_STEER             "Steer"

/// @brief        Read threshold config from local xml file
/// @param p_path The path to the xml file
/// @return       The threshold values
static tDecisionThresholds LoadThresholdSettings(const char* p_path)
{
    tDecisionThresholds thresholds;
    void* paramHandle = GfParmReadFile(p_path, GFPARM_RMODE_REREAD);

    // Get threshold values
    thresholds.Accel = GfParmGetNum(paramHandle, SCT_THRESHOLD, ATT_ACCEL, "%", STANDARD_THRESHOLD_ACCEL);
    thresholds.Brake = GfParmGetNum(paramHandle, SCT_THRESHOLD, ATT_BRAKE, "%", STANDARD_THRESHOLD_BRAKE);
    thresholds.Steer = GfParmGetNum(paramHandle, SCT_THRESHOLD, ATT_STEER, "%", STANDARD_THRESHOLD_STEER);

    // Clamp threshold values
    ClampFloat(&thresholds.Accel, 0.0f, 1.0f);
    ClampFloat(&thresholds.Brake, 0.0f, 1.0f);
    ClampFloat(&thresholds.Steer, 0.0f, 1.0f);

    GfParmReleaseHandle(paramHandle);

    return thresholds;
}
