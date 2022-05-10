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
/// @param p_audio Whether to enable the audio option
/// @param p_icon  Whether to enable the icon option
/// @param p_text  Whether to enable the text option
void IndicatorTest(bool p_audio, bool p_icon, bool p_text)
{
    SDAConfig config;
    tIndicator arr = {p_audio, p_icon, p_text};
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

/// @brief Tests if the SDAConfig sets and gets the participant control settings correctly
/// @param p_intervention Whether to enable participant intervention control
/// @param p_gas Whether to enable participant gas control
/// @param p_steer Whether to enable participant steer control
void PControlTest1(bool p_intervention, bool p_gas, bool p_steer)
{
    SDAConfig config;
    tParticipantControl arr = {p_intervention, p_gas, p_steer, NULL, NULL, NULL};
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

/// @brief                  Tests if the SDAConfig sets and gets the other pControl settings correctly
/// @param p_force          Whether to enable force feedback
/// @param p_userRecord     Whether to enable user controls session recording
/// @param p_blackboxRecord Whether to enable blackbox session recording
void PControlTest2(bool p_force, bool p_userRecord, bool p_blackboxRecord)
{
    SDAConfig config;
    tParticipantControl arr = {NULL, NULL, NULL, p_force, p_userRecord, p_blackboxRecord};
    config.SetPControlSettings(arr);
    tParticipantControl pControl = config.GetPControlSettings();
    ASSERT_EQ(arr.ForceFeedback, pControl.ForceFeedback);
    ASSERT_EQ(arr.RecordSession, pControl.RecordSession);
    ASSERT_EQ(arr.BBRecordSession, pControl.BBRecordSession);
}

/// @brief Tests the SDAConfig ParticipantControlSettings for every possible boolean combination (last 3)
BEGIN_TEST_COMBINATORIAL(ConfigTests, PControlSettings2)
bool booleans[] = {false, true};
END_TEST_COMBINATORIAL3(PControlTest2, booleans, 2, booleans, 2, booleans, 2)

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

/// @brief                Tests if the SDAConfig sets and gets the DataCollectionSetting correctly
/// @param p_env          The environment data setting
/// @param p_car          The car data setting
/// @param p_human        The human data setting
/// @param p_intervention The intervention data setting
/// @param p_meta         The meta data setting
void TestBoolArr(bool p_env, bool p_car, bool p_human, bool p_intervention, bool p_meta)
{
    SDAConfig config;
    tDataToStore arr = {p_env, p_car, p_human, p_intervention, p_meta};
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

void SyncOptionTest(bool p_syncOption)
{
    SDAConfig config;

    config.SetSyncOption(p_syncOption);

    ASSERT_EQ(config.GetSyncOption(), p_syncOption);
}

TEST_CASE(ConfigTests, SyncOptionTrueTest, SyncOptionTest, (true))
TEST_CASE(ConfigTests, SyncOptionFalseTest, SyncOptionTest, (false))

TEST(ConfigTests, ReplayFolderTest)
{
    SDAConfig config;

    char randomPath[64];
    GenerateRandomCharArray(randomPath, 63);

    config.SetReplayFolder(randomPath);
    ASSERT_EQ(config.GetReplayFolder(), randomPath);
}