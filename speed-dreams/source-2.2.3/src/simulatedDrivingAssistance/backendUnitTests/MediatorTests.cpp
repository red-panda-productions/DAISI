#include <gtest/gtest.h>
#include "TestUtils.h"
#include "ConfigEnums.h"
#include "Mediator.h"
#include "Mediator.inl"
#include "SDAConfig.h"
#include "mocks/DecisionMakerMock.h"
#include "mocks/SocketBlackBoxMock.h"
#include "mocks/SQLDatabaseStorageMock.h"
#include "mocks/RecorderMock.h"
#include <sys/types.h>
#include <sys/stat.h>
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING 1
#include <experimental/filesystem>
#include "RppUtils.hpp"
#include "GeneratorUtils.h"

namespace filesystem = std::experimental::filesystem;

/// @brief A mediator that uses the standard SDecisionMakerMock
#define MockMediator Mediator<SDecisionMakerMock>

/// @brief A mediator that uses SDAConfig in DecisionmakerMock internally
#define SDAConfigMediator Mediator<DecisionMakerMock<SDAConfig>>

/// @brief The amount of tests for multiple tests
#define TEST_AMOUNT 20

// @brief The amount of minutes in a day
#define DAY_MINUTES 1440

// @brief The amount of ticks in a day for 0.006 ms per tick (standard)
#define DAY_TICKS 14400000

// @brief Fake path for a test
#define FAKE_PATH "Totally/a/path"

template <>
MockMediator* MockMediator::m_instance = nullptr;

template <>
SDAConfigMediator* SDAConfigMediator::m_instance = nullptr;

/// @brief Test if the distribution of the mediator works
TEST(MediatorTests, GetDistributedMediatorTemplated)
{
    MockMediator::ClearInstance();
    ASSERT_TRUE(SetupSingletonsFolder());

    MockMediator* mediator1 = MockMediator::GetInstance();
    MockMediator* mediator2 = MockMediator::GetInstance();
    ASSERT_EQ(mediator1, mediator2);
    DeleteSingletonsFolder();
}

/// @brief Test if the distribution of the mediator works
TEST(MediatorTests, GetDistributedMediator)
{
    MockMediator::ClearInstance();
    ASSERT_TRUE(SetupSingletonsFolder());

    MockMediator* mediator1 = MockMediator::GetInstance();
    MockMediator* mediator2 = MockMediator::GetInstance();
    ASSERT_EQ(mediator1, mediator2);
    DeleteSingletonsFolder();
}

/// @brief                    Tests if the Mediator sets and gets the interventionType correctly
/// @param p_interventionType The interventionType that needs to be set
void InterventionTestMediator(InterventionType p_interventionType)
{
    ASSERT_TRUE(SetupSingletonsFolder());

    SDAConfigMediator* mediator = SDAConfigMediator::GetInstance();

    mediator->SetInterventionType(p_interventionType);
    ASSERT_EQ(p_interventionType, mediator->GetInterventionType());
    DeleteSingletonsFolder();
}

TEST_CASE(MediatorTests, InterventionTestNoSignals, InterventionTestMediator, (INTERVENTION_TYPE_NO_SIGNALS))
TEST_CASE(MediatorTests, InterventionTestOnlySignals, InterventionTestMediator, (INTERVENTION_TYPE_ONLY_SIGNALS))
TEST_CASE(MediatorTests, InterventionTestSharedControl, InterventionTestMediator, (INTERVENTION_TYPE_SHARED_CONTROL))
TEST_CASE(MediatorTests, InterventionTestCompleteTakeover, InterventionTestMediator, (INTERVENTION_TYPE_COMPLETE_TAKEOVER))

/// @brief                      Tests if the Mediator sets and gets the black box sync option correctly
/// @param p_blackBoxSyncOption The sync option that needs to be set
void BlackBoxSyncOptionTestMediator(bool p_blackBoxSyncOption)
{
    ASSERT_TRUE(SetupSingletonsFolder());

    SDAConfigMediator* mediator = SDAConfigMediator::GetInstance();

    mediator->SetBlackBoxSyncOption(p_blackBoxSyncOption);
    ASSERT_EQ(p_blackBoxSyncOption, mediator->GetBlackBoxSyncOption());

    DeleteSingletonsFolder();
}

TEST_CASE(MediatorTests, SyncOptionTestTrue, BlackBoxSyncOptionTestMediator, (true))
TEST_CASE(MediatorTests, SyncOptionTestFalse, BlackBoxSyncOptionTestMediator, (false))

/// @brief Tests if reading a mediator pointer from a file works
TEST(MediatorTests, ReadFromFile)
{
    MockMediator::ClearInstance();
    ASSERT_TRUE(SetupSingletonsFolder());
    MockMediator* mediator1 = MockMediator::GetInstance();

    MockMediator::ClearInstance(false);  // Our next call should read the pointer to the mediator from a file.
    // We don't want that pointer to point to a deleted mediator, so call with false
    MockMediator* mediator2 = MockMediator::GetInstance();
    ASSERT_EQ(mediator1, mediator2);

    DeleteSingletonsFolder();
}

/// @brief Tests if de mediator gets the threshold settings correctly
void ThresholdTestMediator(float p_accel, float p_brake, float p_steer)
{
    SDAConfigMediator::ClearInstance();
    ASSERT_TRUE(SetupSingletonsFolder());

    SDAConfigMediator* mediator = SDAConfigMediator::GetInstance();

    tDecisionThresholds thresholdsIn{p_accel, p_brake, p_steer};
    mediator->SetThresholdSettings(thresholdsIn);
    tDecisionThresholds thresholdsOut = mediator->GetThresholdSettings();

    ASSERT_EQ(thresholdsIn.Accel, thresholdsOut.Accel);
    ASSERT_EQ(thresholdsIn.Brake, thresholdsOut.Brake);
    ASSERT_EQ(thresholdsIn.Steer, thresholdsOut.Steer);
}
BEGIN_TEST_COMBINATORIAL(MediatorTests, ThresholdTest)
float floatVals[] = {-1, 0, 0.5, 1, 2};
END_TEST_COMBINATORIAL3(ThresholdTestMediator, floatVals, 5, floatVals, 5, floatVals, 5)

/// @brief                    Tests if the Mediator sets and gets the interventionType correctly from SDAConfig
/// @param p_interventionType The interventionType to test for
void InterventionTypeTestMediator(InterventionType p_interventionType)
{
    SDAConfigMediator::ClearInstance();
    ASSERT_TRUE(SetupSingletonsFolder());
    SDAConfigMediator::GetInstance()->SetInterventionType(p_interventionType);
    const InterventionType it = SDAConfigMediator::GetInstance()->GetDecisionMaker()->Type;
    ASSERT_EQ(p_interventionType, it);
}

TEST_CASE(MediatorTests, InterventionTypeTestNoSignals, InterventionTypeTestMediator, (INTERVENTION_TYPE_NO_SIGNALS))
TEST_CASE(MediatorTests, InterventionTypeTestOnlySignals, InterventionTypeTestMediator, (INTERVENTION_TYPE_ONLY_SIGNALS))
TEST_CASE(MediatorTests, InterventionTypeTestSharedControl, InterventionTypeTestMediator, (INTERVENTION_TYPE_SHARED_CONTROL))
TEST_CASE(MediatorTests, InterventionTypeTestCompleteTakeover, InterventionTypeTestMediator, (INTERVENTION_TYPE_COMPLETE_TAKEOVER))

/// @brief              Tests if the mediator sets and gets the allowed actions correctly
/// @param p_steer      Whether the black box can steer
/// @param p_accelerate Whether the black box can give gas
/// @param p_brake      Whether the black box can brake
void AllowedActionsTestMediator(bool p_steer, bool p_accelerate, bool p_brake)
{
    SDAConfigMediator::ClearInstance();
    ASSERT_TRUE(SetupSingletonsFolder());
    tAllowedActions allowedActionsSet = {p_steer, p_accelerate, p_brake};
    SDAConfigMediator::GetInstance()->SetAllowedActions(allowedActionsSet);
    tAllowedActions allowedActionsGet = SDAConfigMediator::GetInstance()->GetAllowedActions();
    ASSERT_EQ(p_steer, allowedActionsGet.Steer);
    ASSERT_EQ(p_accelerate, allowedActionsGet.Accelerate);
    ASSERT_EQ(p_brake, allowedActionsGet.Brake);
}

/// @brief Tests the Mediator allowed actions for every possible combination
BEGIN_TEST_COMBINATORIAL(MediatorTests, AllowedActions)
bool booleans[] = {false, true};
END_TEST_COMBINATORIAL3(AllowedActionsTestMediator, booleans, 2, booleans, 2, booleans, 2)

/// @brief         Tests if the mediator sets and gets the IndicatorSettings correctly
/// @param p_audio Whether to enable the audio option
/// @param p_icon  Whether to enable the icon option
/// @param p_text  Whether to enable the text option
void IndicatorTestMediator(bool p_audio, bool p_icon, bool p_text)
{
    SDAConfigMediator::ClearInstance();
    ASSERT_TRUE(SetupSingletonsFolder());
    tIndicator arr = {p_audio, p_icon, p_text};
    SDAConfigMediator::GetInstance()->SetIndicatorSettings(arr);
    tIndicator indicator = SDAConfigMediator::GetInstance()->GetIndicatorSettings();
    ASSERT_EQ(arr.Audio, indicator.Audio);
    ASSERT_EQ(arr.Icon, indicator.Icon);
    ASSERT_EQ(arr.Text, indicator.Text);
}

/// @brief Tests the Mediator IndicatorSetting for every possible boolean combination
BEGIN_TEST_COMBINATORIAL(MediatorTests, IndicatorSettings)
bool booleans[] = {false, true};
END_TEST_COMBINATORIAL3(IndicatorTestMediator, booleans, 2, booleans, 2, booleans, 2)

/// @brief                Tests if the Mediator sets and gets the participant control settings correctly
/// @param p_intervention Control intervention toggle option
/// @param p_gas          Control gas option
/// @param p_steer        Control steering option
/// @param p_force        Force feedback option
void PControlTestMediator(bool p_steer, bool p_gas, bool p_brake, bool p_intervention, bool p_force)
{
    SDAConfigMediator::ClearInstance();
    ASSERT_TRUE(SetupSingletonsFolder());
    tParticipantControl arr = {p_steer, p_gas, p_brake, p_intervention, p_force};
    SDAConfigMediator::GetInstance()->SetPControlSettings(arr);
    tParticipantControl pControl = SDAConfigMediator::GetInstance()->GetPControlSettings();
    ASSERT_EQ(arr.ControlSteer, pControl.ControlSteer);
    ASSERT_EQ(arr.ControlAccel, pControl.ControlAccel);
    ASSERT_EQ(arr.ControlBrake, pControl.ControlBrake);
    ASSERT_EQ(arr.ControlInterventionToggle, pControl.ControlInterventionToggle);
    ASSERT_EQ(arr.ForceFeedback, pControl.ForceFeedback);
}

/// @brief Tests the Mediator ParticipantControlSettings for every possible boolean combination
BEGIN_TEST_COMBINATORIAL(MediatorTests, PControlSettings)
bool booleans[] = {false, true};
END_TEST_COMBINATORIAL5(PControlTestMediator, booleans, 2, booleans, 2, booleans, 2, booleans, 2, booleans, 2)

/// @brief Tests if the Mediator sets and gets the MaxTime correctly
TEST(MediatorTests, MaxTimeTest)
{
    Random random;
    for (int i = 0; i < TEST_AMOUNT; i++)
    {
        SDAConfigMediator::ClearInstance();
        ASSERT_TRUE(SetupSingletonsFolder());
        int maxTime = random.NextInt();
        SDAConfigMediator::GetInstance()->SetMaxTime(maxTime);
        ASSERT_EQ(maxTime, SDAConfigMediator::GetInstance()->GetMaxTime());
    }
}

/// @brief Tests if the Mediator can check the connection when settings are correct
TEST(MediatorTests, CheckCorrectConnectionTest)
{
    tDatabaseSettings testSettings{"SDATest", "PASSWORD", "127.0.0.1", 3306, "sda_test", false};
    bool connectable = SDAConfigMediator::GetInstance()->CheckConnection(testSettings);
    ASSERT_TRUE(connectable);
}

/// @brief Tests if the Mediator can check the connection when settings are incorrect
TEST(MediatorTests, CheckIncorrectConnectionTest)
{
    tDatabaseSettings testSettings{"SDATest", "WRONGPASSWORD", "127.0.0.1", 3306, "sda_test", false};
    bool connectable = SDAConfigMediator::GetInstance()->CheckConnection(testSettings);
    ASSERT_FALSE(connectable);
}

/// @brief Tests if the Mediator sets and gets the UserID correctly
TEST(MediatorTests, UserIDTest)
{
    Random random;
    char buf[32];
    for (int i = 0; i < TEST_AMOUNT; i++)
    {
        SDAConfigMediator::ClearInstance();
        ASSERT_TRUE(SetupSingletonsFolder());
        int userID = random.NextInt();
        sprintf(buf, "%d", userID);
        SDAConfigMediator::GetInstance()->SetUserId(buf);
        const SDAConfig config = SDAConfigMediator::GetInstance()->GetDecisionMaker()->Config;
        ASSERT_EQ(buf, config.GetUserId());
    }
}

/// @brief Tests if the Mediator sets and gets the compression rate correctly
TEST(MediatorTests, CompressionRateTest)
{
    Random random;
    char buf[32];
    for (int i = 0; i < TEST_AMOUNT; i++)
    {
        SDAConfigMediator::ClearInstance();
        ASSERT_TRUE(SetupSingletonsFolder());
        int compressionRate = random.NextInt();
        SDAConfigMediator::GetInstance()->SetCompressionRate(compressionRate);
        const SDAConfig config = SDAConfigMediator::GetInstance()->GetDecisionMaker()->Config;
        ASSERT_EQ(compressionRate, config.GetCompressionRate());
    }
}

/// @brief Tests if the Mediator sets and gets the BlackBoxFilePath correctly
TEST(MediatorTests, BlackBoxFilePathTest)
{
    Random random;
    char path[256];
    for (int j = 0; j < TEST_AMOUNT; j++)
    {
        SDAConfigMediator::ClearInstance();
        ASSERT_TRUE(SetupSingletonsFolder());
        int length = random.NextInt(256);
        GenerateRandomCharArray(path, length);
        SDAConfigMediator::GetInstance()->SetBlackBoxFilePath(path);
        const SDAConfig config = SDAConfigMediator::GetInstance()->GetDecisionMaker()->Config;
        const char* configPath = config.GetBlackBoxFilePath();
        TestStringEqual(path, configPath, length);
    }
}

/// @brief Tests if the Mediator sets and gets the EnvironmentFilePath correctly
TEST(MediatorTests, EnvironmentFilePathTest)
{
    Random random;
    char path[256];
    for (int j = 0; j < TEST_AMOUNT; j++)
    {
        SDAConfigMediator::ClearInstance();
        ASSERT_TRUE(SetupSingletonsFolder());
        int length = random.NextInt(256);
        GenerateRandomCharArray(path, length);
        SDAConfigMediator::GetInstance()->SetEnvironmentFilePath(path);
        // Check whether the path returned by the mediator is correct
        const char* mediatorPath = SDAConfigMediator::GetInstance()->GetEnvironmentFilePath();
        TestStringEqual(path, mediatorPath, length);
        // Check whether the path is also set in the config
        const SDAConfig config = SDAConfigMediator::GetInstance()->GetDecisionMaker()->Config;
        const char* configPath = config.GetEnvironmentFilePath();
        TestStringEqual(path, configPath, length);
    }
}

/// @brief                Tests if the Mediator sets and gets the DataCollectionSetting correctly
/// @param p_env          The environment data setting
/// @param p_car          The car data setting
/// @param p_human        The human data setting
/// @param p_intervention The intervention data setting
/// @param p_meta         The meta data setting
void TestBoolArrMediator(bool p_env, bool p_car, bool p_human, bool p_intervention, bool p_meta)
{
    SDAConfigMediator::ClearInstance();
    ASSERT_TRUE(SetupSingletonsFolder());
    tDataToStore arr = {p_env, p_car, p_human, p_intervention, p_meta};
    SDAConfigMediator::GetInstance()->SetDataCollectionSettings(arr);
    const SDAConfig config = SDAConfigMediator::GetInstance()->GetDecisionMaker()->Config;
    tDataToStore dataToStore = config.GetDataCollectionSetting();
    ASSERT_EQ(arr.EnvironmentData, dataToStore.EnvironmentData);
    ASSERT_EQ(arr.CarData, dataToStore.CarData);
    ASSERT_EQ(arr.HumanData, dataToStore.HumanData);
    ASSERT_EQ(arr.InterventionData, dataToStore.InterventionData);
    ASSERT_EQ(arr.MetaData, dataToStore.MetaData);
}

/// @brief Tests the Mediator DataCollectionSetting for every possible boolean combination
BEGIN_TEST_COMBINATORIAL(MediatorTests, DataCollectionSettings)
bool booleans[] = {false, true};
END_TEST_COMBINATORIAL5(TestBoolArrMediator, booleans, 2, booleans, 2, booleans, 2, booleans, 2, booleans, 2)

/// @brief                         Tests if the mediator gets and sets the replay recorder option correctly
/// @param p_replayRecorderSetting The replay recorder option
void TestReplayRecorderSettingMediator(bool p_replayRecorderSetting)
{
    SDAConfigMediator::ClearInstance();
    ASSERT_TRUE(SetupSingletonsFolder());
    SDAConfigMediator::GetInstance()->SetReplayRecorderSetting(p_replayRecorderSetting);
    ASSERT_EQ(p_replayRecorderSetting, SDAConfigMediator::GetInstance()->GetReplayRecorderSetting());
}

TEST_CASE(MediatorTests, ReplayRecorderOn, TestReplayRecorderSettingMediator, (true))
TEST_CASE(MediatorTests, ReplayRecorderOff, TestReplayRecorderSettingMediator, (false))

/// @brief Tests if the Mediator sets and gets the Replay folder correctly
TEST(MediatorTests, ReplayFolderTest)
{
    Random random;
    for (int j = 0; j < TEST_AMOUNT; j++)
    {
        char path[256];
        SDAConfigMediator::ClearInstance();
        ASSERT_TRUE(SetupSingletonsFolder());
        int length = random.NextInt(256);
        GenerateRandomCharArray(path, length);
        filesystem::path pathSet = path;
        SDAConfigMediator::GetInstance()->SetReplayFolder(pathSet);
        // filesystem::path has overloaded (==) to lexicographically compare two paths
        ASSERT_TRUE(pathSet == SDAConfigMediator::GetInstance()->GetReplayFolder());
    }
}

/// @brief Tests if the mediator gets and sets the tick count correctly. (For use in the following tests)
TEST(MediatorTests, TickCountTest)
{
    MockMediator::ClearInstance();
    ASSERT_TRUE(SetupSingletonsFolder());
    Random random;
    unsigned long tickCount = random.NextUInt();
    MockMediator::GetInstance()->SetTickCount(tickCount);
    ASSERT_EQ(tickCount, MockMediator::GetInstance()->GetTickCount());
}

/// @brief Tests if the mediator advances the simulation correctly
TEST(MediatorTests, DriveTick)
{
    MockMediator::ClearInstance();
    ASSERT_TRUE(SetupSingletonsFolder());
    tCarElt car = {};
    tSituation situation = {};
    Random random;
    unsigned long tickCount = random.NextUInt();
    MockMediator::GetInstance()->SetTickCount(tickCount);
    MockMediator::GetInstance()->DriveTick(&car, &situation);
    ASSERT_EQ(tickCount + 1, MockMediator::GetInstance()->GetTickCount());
}

/// @brief                         Tests if the mediator starts the race correctly
/// @param p_replayRecorderSetting The replay recorder setting
/// @param p_blackBoxFilePath      The black box file path
void RaceStartTest(bool p_replayRecorderSetting, const char* p_blackBoxFilePath)
{
    MockMediator::ClearInstance();
    ASSERT_TRUE(SetupSingletonsFolder());
    tTrack track = {};
    tSituation situation = {};
    Recorder* recorder = nullptr;
    MockMediator::GetInstance()->SetReplayRecorderSetting(p_replayRecorderSetting);
    MockMediator::GetInstance()->SetBlackBoxFilePath(p_blackBoxFilePath);
    ASSERT_NO_THROW(MockMediator::GetInstance()->RaceStart(&track, nullptr, nullptr, &situation, recorder));
    ASSERT_TRUE(MockMediator::GetInstance()->GetInRace());
}

/// @brief Tests if the mediator starts the race correctly
TEST(MediatorTests, RaceStart)
{
    GfInit();

    Random random;
    for (int i = 0; i < TEST_AMOUNT; i++)
    {
        char blackBoxFilePath[256];
        GenerateRandomCharArray(blackBoxFilePath, random.NextInt(256));
        RaceStartTest(true, blackBoxFilePath);
        RaceStartTest(false, blackBoxFilePath);
    }

    GfShutdown();
}

/// @brief Tests if the mediator stops the race correctly
TEST(MediatorTests, RaceStop)
{
    MockMediator::ClearInstance();
    ASSERT_TRUE(SetupSingletonsFolder());

    MockMediator::GetInstance()->SetInRace(true);
    MockMediator::GetInstance()->GetDecisionMaker()->MStoppedRace = false;
    ASSERT_NO_THROW(MockMediator::GetInstance()->RaceStop());
    ASSERT_FALSE(MockMediator::GetInstance()->GetInRace());
    ASSERT_TRUE(MockMediator::GetInstance()->GetDecisionMaker()->MStoppedRace);

    MockMediator::GetInstance()->SetInRace(false);
    MockMediator::GetInstance()->GetDecisionMaker()->MStoppedRace = false;
    ASSERT_NO_THROW(MockMediator::GetInstance()->RaceStop());
    ASSERT_FALSE(MockMediator::GetInstance()->GetInRace());
    ASSERT_FALSE(MockMediator::GetInstance()->GetDecisionMaker()->MStoppedRace);
}

/// @brief Tests if the TimeOut function returns the correct time out
TEST(MediatorTests, TimeOutTest)
{
    Random random;
    for (int j = 0; j < TEST_AMOUNT; j++)
    {
        SDAConfigMediator::ClearInstance();
        ASSERT_TRUE(SetupSingletonsFolder());

        int maxTimeMinutes = random.NextInt(0, DAY_MINUTES);
        SDAConfigMediator::GetInstance()->GetDecisionMaker()->Config.SetMaxTime(maxTimeMinutes);

        unsigned long currentTick = random.NextUInt(DAY_TICKS);
        SDAConfigMediator::GetInstance()->SetTickCount(currentTick);

        float maxTimeSeconds = static_cast<float>(maxTimeMinutes) * 60;
        float currentTime = static_cast<float>(currentTick) * static_cast<float>(RCM_MAX_DT_ROBOTS);
        bool isTimedOut = maxTimeSeconds < currentTime;
        ASSERT_EQ(SDAConfigMediator::GetInstance()->TimeOut(), isTimedOut);
    }
}

/// @brief tests if you can change the bool value to save to a database to true or to false
TEST(MediatorTests, ChangeSaveToDatabaseValueTest)
{
    SDAConfigMediator::ClearInstance();
    ASSERT_TRUE(SetupSingletonsFolder());
    Random random;

    for (int i = 0; i < 10; i++)
    {
        bool controlBool = random.NextBool();
        SDAConfigMediator::GetInstance()->SetSaveRaceToDatabase(controlBool);
        ASSERT_EQ(SDAConfigMediator::GetInstance()->GetDecisionMaker()->Config.GetSaveToDatabaseCheck(), controlBool);
    }
}