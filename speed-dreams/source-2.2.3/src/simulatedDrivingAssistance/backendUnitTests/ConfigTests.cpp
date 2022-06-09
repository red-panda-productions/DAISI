#include <gtest/gtest.h>
#include "TestUtils.h"
#include "SDAConfig.h"
#include "ConfigEnums.h"
#include "Random.hpp"
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
TEST_CASE(ConfigTests, InterventionTypeTestAutonomousAI, InterventionTypeTest, (INTERVENTION_TYPE_AUTONOMOUS_AI))

/// @brief              Tests if the SDAConfig sets and gets the allowed actions correctly
/// @param p_steer      Whether the black box can steer
/// @param p_accelerate Whether the black box can give gas
/// @param p_brake      Whether the black box can brake
void AllowedActionsTestConfig(bool p_steer, bool p_accelerate, bool p_brake)
{
    SDAConfig config;
    tAllowedActions allowedActionsSet = {p_steer, p_accelerate, p_brake};
    config.SetAllowedActions(allowedActionsSet);

    tAllowedActions allowedActionsGet = config.GetAllowedActions();
    ASSERT_EQ(p_steer, allowedActionsGet.Steer);
    ASSERT_EQ(p_accelerate, allowedActionsGet.Accelerate);
    ASSERT_EQ(p_brake, allowedActionsGet.Brake);
}

/// @brief Tests the SDAConfig allowed actions for every possible combination
BEGIN_TEST_COMBINATORIAL(ConfigTests, AllowedActions)
bool booleans[] = {false, true};
END_TEST_COMBINATORIAL3(AllowedActionsTestConfig, booleans, 2, booleans, 2, booleans, 2)

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

/// @brief                Tests if the SDAConfig sets and gets the participant control settings correctly
/// @param p_steer        Whether to enable participant steer control
/// @param p_gas          Whether to enable participant gas control
/// @param p_brake        Whether to enable participant brake control
/// @param p_intervention Whether to enable participant intervention control
/// @param p_force        Whether to enable force feedback
void PControlTest(bool p_steer, bool p_gas, bool p_brake, bool p_intervention)
{
    SDAConfig config;
    tParticipantControl arr = {p_steer, p_gas, p_brake, p_intervention};
    config.SetPControlSettings(arr);
    tParticipantControl pControl = config.GetPControlSettings();
    ASSERT_EQ(arr.ControlSteer, pControl.ControlSteer);
    ASSERT_EQ(arr.ControlAccel, pControl.ControlAccel);
    ASSERT_EQ(arr.ControlBrake, pControl.ControlBrake);
    ASSERT_EQ(arr.ControlInterventionToggle, pControl.ControlInterventionToggle);
}

/// @brief Tests the SDAConfig ParticipantControlSettings for every possible boolean combination (first 3)
BEGIN_TEST_COMBINATORIAL(ConfigTests, PControlSettings)
bool booleans[] = {false, true};
END_TEST_COMBINATORIAL5(PControlTest, booleans, 2, booleans, 2, booleans, 2, booleans, 2, booleans, 2)

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

/// @brief Tests if the SDAConfig sets and gets the CompressionRate correctly
TEST(ConfigTests, CompressionRateTest)
{
    SDAConfig config;
    Random random;

    for (int i = 0; i < 20; i++)
    {
        int compressionRate = random.NextInt();
        config.SetCompressionRate(compressionRate);
        ASSERT_EQ(compressionRate, config.GetCompressionRate());
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
        GenerateRandomCharArray(path, length - 1);
        config.SetBlackBoxFilePath(path);
        const char* configPath = config.GetBlackBoxFilePath();
        ASSERT_STREQ(path, configPath);
    }
}

/// @brief Tests if the SDAConfig sets and gets the EnvironmentFilePath correctly
TEST(ConfigTests, EnvironmentFilePathTest)
{
    SDAConfig config;
    Random random;
    for (int j = 0; j <= 10; j++)
    {
        char path[256];
        int length = 2 + 254 * j / 10;
        GenerateRandomCharArray(path, length - 1);
        config.SetEnvironmentFilePath(path);
        const char* configPath = config.GetEnvironmentFilePath();
        ASSERT_STREQ(path, configPath);
    }
}

void BlackBoxSyncOptionTestConfig(bool p_sync)
{
    SDAConfig config;
    config.SetBlackBoxSyncOption(p_sync);
    ASSERT_EQ(p_sync, config.GetBlackBoxSyncOption());
}

TEST_CASE(ConfigTests, BlackBoxSyncOptionTestAsync, BlackBoxSyncOptionTestConfig, (true))
TEST_CASE(ConfigTests, BlackBoxSyncOptionTestSync, BlackBoxSyncOptionTestConfig, (false))

/// @brief                Tests if the SDAConfig sets and gets the DataCollectionSetting correctly
/// @param p_env          The environment data setting
/// @param p_car          The car data setting
/// @param p_human        The human data setting
/// @param p_intervention The intervention data setting
void TestBoolArr(bool p_env, bool p_car, bool p_human, bool p_intervention)
{
    SDAConfig config;
    tDataToStore arr = {p_env, p_car, p_human, p_intervention};
    config.SetDataCollectionSettings(arr);
    tDataToStore dataToStore = config.GetDataCollectionSetting();
    ASSERT_EQ(arr.EnvironmentData, dataToStore.EnvironmentData);
    ASSERT_EQ(arr.CarData, dataToStore.CarData);
    ASSERT_EQ(arr.HumanData, dataToStore.HumanData);
    ASSERT_EQ(arr.InterventionData, dataToStore.InterventionData);
}

/// @brief Tests the SDAConfig DataCollectionSetting for every possible boolean combination
BEGIN_TEST_COMBINATORIAL(ConfigTests, DataCollectionSettings)
bool booleans[] = {false, true};
END_TEST_COMBINATORIAL4(TestBoolArr, booleans, 2, booleans, 2, booleans, 2, booleans, 2)

TEST(ConfigTests, ReplayFolderTest)
{
    SDAConfig config;

    char randomPath[64];
    GenerateRandomCharArray(randomPath, 63);

    config.SetReplayFolder(randomPath);
    ASSERT_EQ(config.GetReplayFolder(), randomPath);
}

/// @brief        Tests if the saveToDatabse bool gets correctly changed in the config file
/// @param p_save boolean that returns decides if the data should or should not be saved.
void SaveRaceToDatabaseTestConf(bool p_save)
{
    SDAConfig config;

    config.SetSaveToDatabaseCheck(p_save);
    ASSERT_EQ(p_save, config.GetSaveToDatabaseCheck());
}

TEST_CASE(ConfigTests, SaveDataToDatabase, SaveRaceToDatabaseTestConf, (true))
TEST_CASE(ConfigTests, DontSaveDataToDatabase, SaveRaceToDatabaseTestConf, (false))