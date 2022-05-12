#include <gtest/gtest.h>
#include "TestUtils.h"
#include "ThresholdConfig.h"

/// @brief Tests if de threshold config gets the threshold settings correctly from xml files
TEST(ThresholdConfigTests, LoadThresholdSettingsTest)
{
    GfInit();

    // Create Data Directory if not already done.
    GfSetDataDir(SD_DATADIR_SRC);

    // Check if mediator returns standard values from the xml
    std::string dstStr1("../test_data/test_thresholds_defaults.xml");
    char path[512];
    sprintf(path, "%s%s", GfDataDir(), dstStr1.c_str());

    tDecisionThresholds thresholds = LoadThresholdSettings(path);
    ASSERT_ALMOST_EQ(thresholds.Accel, STANDARD_THRESHOLD_ACCEL, 0.000001);
    ASSERT_ALMOST_EQ(thresholds.Brake, STANDARD_THRESHOLD_BRAKE, 0.000001);
    ASSERT_ALMOST_EQ(thresholds.Steer, STANDARD_THRESHOLD_STEER, 0.000001);

    // Check if mediator returns other values if values have been set
    std::string dstStr2("../test_data/test_thresholds.xml");
    sprintf(path, "%s%s", GfDataDir(), dstStr2.c_str());
    void* paramHandle = GfParmReadFile(path, GFPARM_RMODE_REREAD);

    tDecisionThresholds xmlThresholds;
    xmlThresholds.Accel = GfParmGetNum(paramHandle, "Threshold Settings", "Accel", "%", 0);
    xmlThresholds.Brake = GfParmGetNum(paramHandle, "Threshold Settings", "Brake", "%", 0);
    xmlThresholds.Steer = GfParmGetNum(paramHandle, "Threshold Settings", "Steer", "%", 0);

    thresholds = LoadThresholdSettings(path);

    ASSERT_EQ(thresholds.Accel, xmlThresholds.Accel);
    ASSERT_EQ(thresholds.Brake, xmlThresholds.Brake);
    ASSERT_EQ(thresholds.Steer, xmlThresholds.Steer);
}
