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
#include <experimental/filesystem>

namespace filesystem = std::experimental::filesystem;

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

/// @brief Tests if de mediator sets and gets the threshold settings correctly
/// Fails if the a local threshold settings xml exists with values different from the defaults
/// Succeeds if the file has not been changed or does not exist
TEST(MediatorTests, GetThresholdTest)
{
    // Clear old Mediator instance
    SMediator::ClearInstance();
    ASSERT_TRUE(SetupSingletonsFolder());

    GfInit();
    GfSetLocalDir(SD_LOCALDIR);

    // Check if mediator returns standard values if no xml is found
    SMediator* mediator = SMediator::GetInstance();
    tThreshold thresholds = mediator->GetThresholdSettings();
    ASSERT_ALMOST_EQ(thresholds.Accel, STANDARD_THRESHOLD_ACCEL, 0.000001);
    ASSERT_ALMOST_EQ(thresholds.Brake, STANDARD_THRESHOLD_BRAKE, 0.000001);
    ASSERT_ALMOST_EQ(thresholds.Steer, STANDARD_THRESHOLD_STEER, 0.000001);

    // Check if mediator returns standard values if values have been set
    thresholds = mediator->GetThresholdSettings();
    ASSERT_ALMOST_EQ(thresholds.Accel, STANDARD_THRESHOLD_ACCEL, 0.000001);
    ASSERT_ALMOST_EQ(thresholds.Brake, STANDARD_THRESHOLD_BRAKE, 0.000001);
    ASSERT_ALMOST_EQ(thresholds.Steer, STANDARD_THRESHOLD_STEER, 0.000001);

    // Test if the test_thresholds.xml file exists
    std::string path("test_data");
    if (!FindFileDirectory(path, "test_thresholds.xml")) throw std::exception("Can't find test file");

    // Create Data Directory if not already done.
    GfSetDataDir(SD_DATADIR_SRC);

    // Test if the mediator returns the right thresholds when it reads from a xml file
    std::string dstStr("../test_data/test_thresholds.xml");
    char buf[512];
    sprintf(buf, "%s%s", GfDataDir(), dstStr.c_str());
    void* paramHandle = GfParmReadFile(buf, GFPARM_RMODE_REREAD | GFPARM_RMODE_CREAT);

    Thresholds xmlThresholds;
    xmlThresholds.Accel = GfParmGetNum(paramHandle, "Threshold Settings", "Accel", "%", 0);
    xmlThresholds.Brake = GfParmGetNum(paramHandle, "Threshold Settings", "Brake", "%", 0);
    xmlThresholds.Steer = GfParmGetNum(paramHandle, "Threshold Settings", "Steer", "%", 0);

    mediator->SetThresholdSettings(buf);
    thresholds = mediator->GetThresholdSettings();

    ASSERT_EQ(thresholds.Accel, xmlThresholds.Accel);
    ASSERT_EQ(thresholds.Brake, xmlThresholds.Brake);
    ASSERT_EQ(thresholds.Steer, xmlThresholds.Steer);
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
    const SDAConfig config = SDAConfigMediator::GetInstance()->GetDecisionMaker()->Config;
    tIndicator indicator = config.GetIndicatorSettings();
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
    const SDAConfig config = SDAConfigMediator::GetInstance()->GetDecisionMaker()->Config;
    tParticipantControl pControl = config.GetPControlSettings();
    ASSERT_EQ(arr.ControlInterventionToggle, pControl.ControlInterventionToggle);
    ASSERT_EQ(arr.ControlSteering, pControl.ControlSteering);
    ASSERT_EQ(arr.ControlGas, pControl.ControlGas);

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
