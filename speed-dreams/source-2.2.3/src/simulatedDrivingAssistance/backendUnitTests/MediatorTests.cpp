#include <gtest/gtest.h>
#include "TestUtils.h"
#include "ConfigEnums.h"
#include "Mediator.h"
#include "Mediator.inl"
#include "SDAConfig.h"
#include "mocks/DecisionMakerMock.h"
#include <sys/types.h>
#include <sys/stat.h>
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING 1
#include <experimental/filesystem>
#include "../rppUtils/RppUtils.hpp"
#include "GeneratorUtils.h"

/// @brief A mediator that uses the standard SDecisionMakerMock
#define MockMediator Mediator<SDecisionMakerMock>

/// @brief A mediator that uses SDAConfig in DecisionmakerMock internally
#define SDAConfigMediator Mediator<DecisionMakerMock<SDAConfig>>

/// @brief The amount of tests for multiple tests
#define TEST_AMOUNT 20

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

    MockMediator::ClearInstance();
    MockMediator* mediator2 = MockMediator::GetInstance();
    ASSERT_EQ(mediator1, mediator2);

    DeleteSingletonsFolder();
}

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

/// @brief        Tests if the Mediator sets and gets the task correctly
/// @param p_task The task to test for
void TaskTestMediator(Task p_task)
{
    SDAConfigMediator::ClearInstance();
    ASSERT_TRUE(SetupSingletonsFolder());
    SDAConfigMediator::GetInstance()->SetTask(p_task);
    const SDAConfig config = SDAConfigMediator::GetInstance()->GetDecisionMaker()->Config;
    ASSERT_EQ(p_task, config.GetTask());
}

TEST_CASE(MediatorTests, TaskTestsLaneKeeping, TaskTestMediator, (TASK_LANE_KEEPING))
TEST_CASE(MediatorTests, TaskTestsSpeedControl, TaskTestMediator, (TASK_SPEED_CONTROL))

/// @brief         Tests if the SDAConfig sets and gets the IndicatorSettings correctly
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
void PControlTestMediator(bool p_intervention, bool p_gas, bool p_steer, bool p_force)
{
    SDAConfigMediator::ClearInstance();
    ASSERT_TRUE(SetupSingletonsFolder());
    tParticipantControl arr = {p_intervention, p_gas, p_steer, p_force};
    SDAConfigMediator::GetInstance()->SetPControlSettings(arr);
    tParticipantControl pControl = SDAConfigMediator::GetInstance()->GetPControlSettings();
    ASSERT_EQ(arr.ControlInterventionToggle, pControl.ControlInterventionToggle);
    ASSERT_EQ(arr.ControlSteering, pControl.ControlSteering);
    ASSERT_EQ(arr.ControlGas, pControl.ControlGas);

/// @brief Tests the Mediator ParticipantControlSettings for every possible boolean combination (first 3)
BEGIN_TEST_COMBINATORIAL(MediatorTests, PControlSettings1)
bool booleans[] = {false, true};
END_TEST_COMBINATORIAL3(PControlTest1Mediator, booleans, 2, booleans, 2, booleans, 2)

/// @brief                  Tests if the SDAConfig sets and gets the other pControl settings correctly
/// @param p_force          Whether to enable force feedback
/// @param p_userRecord     Whether to enable user controls session recording
/// @param p_blackboxRecord Whether to enable blackbox session recording
void PControlTest2Mediator(bool p_force, bool p_record, bool p_blackboxRecord)
{
    SDAConfigMediator::ClearInstance();
    ASSERT_TRUE(SetupSingletonsFolder());
    tParticipantControl arr = {NULL, NULL, NULL, p_force, p_record, p_blackboxRecord};
    SDAConfigMediator::GetInstance()->SetPControlSettings(arr);
    tParticipantControl pControl = SDAConfigMediator::GetInstance()->GetPControlSettings();
    ASSERT_EQ(arr.ForceFeedback, pControl.ForceFeedback);
}

/// @brief Tests the Mediator ParticipantControlSettings for every possible boolean combination
BEGIN_TEST_COMBINATORIAL(MediatorTests, PControlSettings1)
bool booleans[] = {false, true};
END_TEST_COMBINATORIAL4(PControlTestMediator, booleans, 2, booleans, 2, booleans, 2, booleans, 2)

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
        const SDAConfig config = SDAConfigMediator::GetInstance()->GetDecisionMaker()->Config;
        ASSERT_EQ(maxTime, config.GetMaxTime());
    }
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

TEST(MediatorTests, DriveTickTest)
{
    SDAConfigMediator::ClearInstance();
    ASSERT_TRUE(SetupSingletonsFolder());

    TestSegments segments = GenerateSegments();
    tCarElt car = GenerateCar(segments);
    tSituation situation = GenerateSituation();

    SDAConfigMediator::GetInstance()->DriveTick(&car, &situation);

    const tCarElt* checkCar = SDAConfigMediator::GetInstance()->CarController.GetCar();
    ASSERT_TRUE(checkCar == &car);

    DecisionMakerMock<SDAConfig>* decisionMaker = SDAConfigMediator::GetInstance()->GetDecisionMaker();

    ASSERT_TRUE(&car == decisionMaker->Car);
    ASSERT_TRUE(&situation == decisionMaker->Situation);
    ASSERT_TRUE(decisionMaker->TickCount == 0);
}

#define FAKE_PATH "Totally/a/path"

TEST(MediatorTests, RaceStartTest)
{
    GfInit();
    SDAConfigMediator::ClearInstance();
    ASSERT_TRUE(SetupSingletonsFolder());

    DecisionMakerMock<SDAConfig>* decisionMaker = SDAConfigMediator::GetInstance()->GetDecisionMaker();

    tTrack track = {};

    tSituation situation = GenerateSituation();

    SDAConfigMediator::GetInstance()->SetBlackBoxFilePath(FAKE_PATH);
    SDAConfigMediator::GetInstance()->RaceStart(&track, nullptr, nullptr, &situation, nullptr);

    ASSERT_TRUE(decisionMaker->Situation == &situation);
    ASSERT_TRUE(decisionMaker->Track == &track);
    TestStringEqual(FAKE_PATH, decisionMaker->BlackboxExecutablePath.c_str(), decisionMaker->BlackboxExecutablePath.size());
    ASSERT_TRUE(decisionMaker->Recorder == nullptr);
}

TEST(MediatorTests, RaceStopTest)
{
    GfInit();
    SDAConfigMediator::ClearInstance();
    ASSERT_TRUE(SetupSingletonsFolder());

    DecisionMakerMock<SDAConfig>* decisionMaker = SDAConfigMediator::GetInstance()->GetDecisionMaker();
    ASSERT_FALSE(decisionMaker->RaceStopped);

    SDAConfigMediator::GetInstance()->RaceStop();

    ASSERT_FALSE(decisionMaker->RaceStopped);

    tTrack track = {};

    tSituation situation = GenerateSituation();

    SDAConfigMediator::GetInstance()->RaceStart(&track, nullptr, nullptr, &situation, nullptr);

    SDAConfigMediator::GetInstance()->RaceStop();

    ASSERT_TRUE(decisionMaker->RaceStopped);
}