#include <gtest/gtest.h>
#include "TestUtils.h"
#include "SDAConfig.h"
#include "ConfigEnums.h"
#include "../rppUtils/Random.hpp"
#define INTERVENTION_TYPE_AMOUNT 5

/// @brief                    Tests if the SDAConfig sets and gets the interventionType correctly
/// @param p_interventionType The interventionType to test for
void InterventionTypeTest(InterventionType p_interventionType)
{
    SDAConfig config;

    config.SetInterventionType(p_interventionType);
    ASSERT_EQ(p_interventionType, config.GetInterventionType());
}

TEST_CASE(ConfigTests, InterventionTypeTestNoSignals, InterventionTypeTest, (INTERVENTION_TYPE_NO_SIGNALS))
TEST_CASE(ConfigTests, InterventionTypeTestOnlySignals, InterventionTypeTest, (INTERVENTION_TYPE_ONLY_SIGNALS))
TEST_CASE(ConfigTests, InterventionTypeTestSharedControl, InterventionTypeTest, (INTERVENTION_TYPE_SHARED_CONTROL))
TEST_CASE(ConfigTests, InterventionTypeTestCompleteTakeover, InterventionTypeTest, (INTERVENTION_TYPE_COMPLETE_TAKEOVER))

/// @brief        Tests if the SDAConfig sets and gets the task correctly
/// @param p_task The task to test for
void TaskTest(Task p_task)
{
    SDAConfig config;

    config.SetTask(p_task);
    ASSERT_EQ(p_task, config.GetTask());
}

TEST_CASE(ConfigTests, TaskTestsLaneKeeping, TaskTest, (TASK_LANE_KEEPING))
TEST_CASE(ConfigTests, TaskTestsSpeedControl, TaskTest, (TASK_SPEED_CONTROL))

/// @brief         Tests if the SDAConfig sets and gets the IndicatorSettings correctly
/// @param p_bool1 First bool
/// @param p_bool2 Second bool
void IndicatorTest(bool p_bool1, bool p_bool2, bool p_bool3)
{
    SDAConfig config;
    tIndicator arr = {p_bool1, p_bool2, p_bool3};
    config.SetIndicatorSettings(arr);
    tIndicator indicator = config.GetIndicatorSettings();
    ASSERT_EQ(arr.Audio, indicator.Audio);
    ASSERT_EQ(arr.Icon, indicator.Icon);
    ASSERT_EQ(arr.Text, indicator.Text);
}

/// @brief Tests the SDAConfig IndicatorSetting for every possible boolean combination
BEGIN_TEST_COMBINATORIAL(ConfigTests, IndicatorSettings)
bool booleans[] = {false, true};
END_TEST_COMBINATORIAL3(IndicatorTest, booleans, 2, booleans, 2, booleans, 2)

/// @brief         Tests if the SDAConfig sets and gets the participant control settings correctly
/// @param p_bool1 First  bool
/// @param p_bool2 Second bool
/// @param p_bool3 Third  bool
void PControlTest1(bool p_bool1, bool p_bool2, bool p_bool3)
{
    SDAConfig config;
    tParticipantControl arr = {p_bool1, p_bool2, p_bool3, NULL};
    config.SetPControlSettings(arr);
    tParticipantControl pControl = config.GetPControlSettings();
    ASSERT_EQ(arr.ControlInterventionToggle, pControl.ControlInterventionToggle);
    ASSERT_EQ(arr.ControlSteering, pControl.ControlSteering);
    ASSERT_EQ(arr.ControlGas, pControl.ControlGas);
}

/// @brief Tests the SDAConfig ParticipantControlSettings for every possible boolean combination (first 3)
BEGIN_TEST_COMBINATORIAL(ConfigTests, PControlSettings1)
bool booleans[] = {false, true};
END_TEST_COMBINATORIAL3(PControlTest1, booleans, 2, booleans, 2, booleans, 2)

/// @brief         Tests if the SDAConfig sets and gets the other pControl settings correctly
/// @param p_bool1 First  bool
void PControlTest2(bool p_bool1)
{
    SDAConfig config;
    tParticipantControl arr = {NULL, NULL, NULL, p_bool1};
    config.SetPControlSettings(arr);
    tParticipantControl pControl = config.GetPControlSettings();
    ASSERT_EQ(arr.ForceFeedback, pControl.ForceFeedback);
}

/// @brief Tests the SDAConfig ParticipantControlSettings for every possible boolean combination (last 1)
TEST_CASE(ConfigTests, PControlSettings2TestTrue, PControlTest2, (true))
TEST_CASE(ConfigTests, PControlSettings2TestFalse, PControlTest2, (false))

/// @brief                   Tests if the SDAConfig sets and gets the replay recorder status correctly
/// @param p_recorderSetting The recorder setting
void RecorderSettingTest(bool p_recorderSetting)
{
    SDAConfig config;
    config.SetReplayRecorderSetting(p_recorderSetting);
    ASSERT_EQ(p_recorderSetting, config.GetReplayRecorderSetting());
}

/// @brief Tests the SDAConfig recorder settings
TEST_CASE(ConfigTests, RecorderSettingTestTrue, RecorderSettingTest, (true))
TEST_CASE(ConfigTests, RecorderSettingTestFalse, RecorderSettingTest, (false))

/// @brief Tests if the SDAConfig sets and gets the MaxTime correctly
TEST(ConfigTests, MaxTimeTest)
{
    SDAConfig config;
    Random random;

    for (int i = 0; i < 20; i++)
    {
        int maxTime = random.NextInt();
        config.SetMaxTime(maxTime);
        ASSERT_EQ(maxTime, config.GetMaxTime());
    }
}

/// @brief Tests if the SDAConfig sets and gets the UserID correctly
TEST(ConfigTests, UserIDTest)
{
    SDAConfig config;
    Random random;
    char buf[32];

    for (int i = 0; i < 20; i++)
    {
        int userID = random.NextInt();
        sprintf(buf, "%d", userID);
        config.SetUserId(buf);
        ASSERT_EQ(buf, config.GetUserId());
    }
}

/// @brief Tests if the SDAConfig sets and gets the BlackBoxFilePath correctly
TEST(ConfigTests, BlackBoxFilePathTest)
{
    SDAConfig config;
    Random random;
    for (int j = 0; j <= 10; j++)
    {
        char path[256];
        int length = 2 + 254 * j / 10;
        for (int i = 0; i < length - 1; i++)
        {
            char c;
            do
            {  // Make sure no character is assigned the null character
                c = (char)random.NextByte();
            } while (c == '\0');
            path[i] = c;
        }
        path[length - 1] = '\0';
        config.SetBlackBoxFilePath(path);
        const char* configPath = config.GetBlackBoxFilePath();
        for (int i = 0; i < length; i++)
        {
            ASSERT_EQ(path[i], configPath[i]);
        }
    }
}

void SyncOptionTest(bool p_sync)
{
    SDAConfig config;
    config.SetSyncOption(p_sync);
    ASSERT_EQ(p_sync, config.GetSyncOption());
}

TEST_CASE(ConfigTests, SyncOptionTestAsync, SyncOptionTest, (true))
TEST_CASE(ConfigTests, SyncOptionTestSync, SyncOptionTest, (false))

/// @brief         Tests if the SDAConfig sets and gets the DataCollectionSetting correctly
/// @param p_bool1 First bool
/// @param p_bool2 Second bool
/// @param p_bool3 Third bool
/// @param p_bool4 Fourth bool
/// @param p_bool5 Fifth bool
void TestBoolArr(bool p_bool1, bool p_bool2, bool p_bool3, bool p_bool4, bool p_bool5)
{
    SDAConfig config;
    tDataToStore arr = {p_bool1, p_bool2, p_bool3, p_bool4, p_bool5};
    config.SetDataCollectionSettings(arr);
    tDataToStore dataToStore = config.GetDataCollectionSetting();
    ASSERT_EQ(arr.EnvironmentData, dataToStore.EnvironmentData);
    ASSERT_EQ(arr.CarData, dataToStore.CarData);
    ASSERT_EQ(arr.HumanData, dataToStore.HumanData);
    ASSERT_EQ(arr.InterventionData, dataToStore.InterventionData);
    ASSERT_EQ(arr.MetaData, dataToStore.MetaData);
}

/// @brief Tests the SDAConfig DataCollectionSetting for every possible boolean combination
BEGIN_TEST_COMBINATORIAL(ConfigTests, DataCollectionSettings)
bool booleans[] = {false, true};
END_TEST_COMBINATORIAL5(TestBoolArr, booleans, 2, booleans, 2, booleans, 2, booleans, 2, booleans, 2)