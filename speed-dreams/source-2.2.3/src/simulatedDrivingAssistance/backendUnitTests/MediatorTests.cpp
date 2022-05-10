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

#define MockMediator Mediator<SDecisionMakerMock>

#define MockMediator2 Mediator<DecisionMakerMock<SDAConfig>>

template <>
MockMediator* MockMediator::m_instance = nullptr;

template <>
MockMediator2* MockMediator2::m_instance = nullptr;

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

    MockMediator2* mediator = MockMediator2::GetInstance();

    mediator->SetInterventionType(p_interventionType);
    ASSERT_EQ(p_interventionType, mediator->GetInterventionType());
    DeleteSingletonsFolder();
}

TEST_CASE(MediatorTests, InterventionTestNoSignals, InterventionTestMediator, (INTERVENTION_TYPE_NO_SIGNALS))
TEST_CASE(MediatorTests, InterventionTestOnlySignals, InterventionTestMediator, (INTERVENTION_TYPE_ONLY_SIGNALS))
TEST_CASE(MediatorTests, InterventionTestSharedControl, InterventionTestMediator, (INTERVENTION_TYPE_SHARED_CONTROL))
TEST_CASE(MediatorTests, InterventionTestCompleteTakeover, InterventionTestMediator, (INTERVENTION_TYPE_COMPLETE_TAKEOVER))

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
    MockMediator2::ClearInstance();
    ASSERT_TRUE(SetupSingletonsFolder());
    MockMediator2::GetInstance()->SetInterventionType(p_interventionType);
    const InterventionType it = MockMediator2::GetInstance()->GetDecisionMaker()->Type;
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
    MockMediator2::ClearInstance();
    ASSERT_TRUE(SetupSingletonsFolder());
    MockMediator2::GetInstance()->SetTask(p_task);
    const SDAConfig config = MockMediator2::GetInstance()->GetDecisionMaker()->Config;
    ASSERT_EQ(p_task, config.GetTask());
}

TEST_CASE(MediatorTests, TaskTestsLaneKeeping, TaskTestMediator, (TASK_LANE_KEEPING))
TEST_CASE(MediatorTests, TaskTestsSpeedControl, TaskTestMediator, (TASK_SPEED_CONTROL))

/// @brief         Tests if the Mediator sets and gets the IndicatorSettings correctly
/// @param p_bool1 First bool
/// @param p_bool2 Second bool
void IndicatorTestMediator(bool p_bool1, bool p_bool2, bool p_bool3)
{
    MockMediator2::ClearInstance();
    ASSERT_TRUE(SetupSingletonsFolder());
    tIndicator arr = {p_bool1, p_bool2, p_bool3};
    MockMediator2::GetInstance()->SetIndicatorSettings(arr);
    const SDAConfig config = MockMediator2::GetInstance()->GetDecisionMaker()->Config;
    tIndicator indicator = config.GetIndicatorSettings();
    ASSERT_EQ(arr.Audio, indicator.Audio);
    ASSERT_EQ(arr.Icon, indicator.Icon);
    ASSERT_EQ(arr.Text, indicator.Text);
}

/// @brief Tests the Mediator IndicatorSetting for every possible boolean combination
BEGIN_TEST_COMBINATORIAL(MediatorTests, IndicatorSettings)
bool booleans[] = {false, true};
END_TEST_COMBINATORIAL3(IndicatorTestMediator, booleans, 2, booleans, 2, booleans, 2)

/// @brief         Tests if the Mediator sets and gets the participant control settings correctly
/// @param p_bool1 First  bool
/// @param p_bool2 Second bool
/// @param p_bool3 Third  bool
void PControlTest1Mediator(bool p_bool1, bool p_bool2, bool p_bool3)
{
    MockMediator2::ClearInstance();
    ASSERT_TRUE(SetupSingletonsFolder());
    tParticipantControl arr = {p_bool1, p_bool2, p_bool3, NULL, NULL, NULL};
    MockMediator2::GetInstance()->SetPControlSettings(arr);
    const SDAConfig config = MockMediator2::GetInstance()->GetDecisionMaker()->Config;
    tParticipantControl pControl = config.GetPControlSettings();
    ASSERT_EQ(arr.ControlInterventionToggle, pControl.ControlInterventionToggle);
    ASSERT_EQ(arr.ControlSteering, pControl.ControlSteering);
    ASSERT_EQ(arr.ControlGas, pControl.ControlGas);
}

/// @brief Tests the Mediator ParticipantControlSettings for every possible boolean combination (first 3)
BEGIN_TEST_COMBINATORIAL(MediatorTests, PControlSettings1)
bool booleans[] = {false, true};
END_TEST_COMBINATORIAL3(PControlTest1Mediator, booleans, 2, booleans, 2, booleans, 2)

/// @brief         Tests if the v sets and gets the other pControl settings correctly
/// @param p_bool1 First  bool
/// @param p_bool2 Second bool
/// @param p_bool3 Third  bool
void PControlTest2Mediator(bool p_bool1, bool p_bool2, bool p_bool3)
{
    MockMediator2::ClearInstance();
    ASSERT_TRUE(SetupSingletonsFolder());
    tParticipantControl arr = {NULL, NULL, NULL, p_bool1, p_bool2, p_bool3};
    MockMediator2::GetInstance()->SetPControlSettings(arr);
    const SDAConfig config = MockMediator2::GetInstance()->GetDecisionMaker()->Config;
    tParticipantControl pControl = config.GetPControlSettings();
    ASSERT_EQ(arr.ForceFeedback, pControl.ForceFeedback);
    ASSERT_EQ(arr.RecordSession, pControl.RecordSession);
    ASSERT_EQ(arr.BBRecordSession, pControl.BBRecordSession);
}

/// @brief Tests the Mediator ParticipantControlSettings for every possible boolean combination (last 3)
BEGIN_TEST_COMBINATORIAL(MediatorTests, PControlSettings2)
bool booleans[] = {false, true};
END_TEST_COMBINATORIAL3(PControlTest2Mediator, booleans, 2, booleans, 2, booleans, 2)

/// @brief Tests if the Mediator sets and gets the MaxTime correctly
TEST(MediatorTests, MaxTimeTest)
{
    Random random;
    for (int i = 0; i < 20; i++)
    {
        MockMediator2::ClearInstance();
        ASSERT_TRUE(SetupSingletonsFolder());
        int maxTime = random.NextInt();
        MockMediator2::GetInstance()->SetMaxTime(maxTime);
        const SDAConfig config = MockMediator2::GetInstance()->GetDecisionMaker()->Config;
        ASSERT_EQ(maxTime, config.GetMaxTime());
    }
}

/// @brief Tests if the Mediator sets and gets the UserID correctly
TEST(MediatorTests, UserIDTest)
{
    Random random;
    char buf[32];
    for (int i = 0; i < 20; i++)
    {
        MockMediator2::ClearInstance();
        ASSERT_TRUE(SetupSingletonsFolder());
        int userID = random.NextInt();
        sprintf(buf, "%d", userID);
        MockMediator2::GetInstance()->SetUserId(buf);
        const SDAConfig config = MockMediator2::GetInstance()->GetDecisionMaker()->Config;
        ASSERT_EQ(buf, config.GetUserId());
    }
}

/// @brief Tests if the Mediator sets and gets the BlackBoxFilePath correctly
TEST(MediatorTests, BlackBoxFilePathTest)
{
    Random random;
    for (int j = 0; j <= 10; j++)
    {
        MockMediator2::ClearInstance();
        ASSERT_TRUE(SetupSingletonsFolder());
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
        MockMediator2::GetInstance()->SetBlackBoxFilePath(path);
        const SDAConfig config = MockMediator2::GetInstance()->GetDecisionMaker()->Config;
        const char* configPath = config.GetBlackBoxFilePath();
        for (int i = 0; i < length; i++)
        {
            ASSERT_EQ(path[i], configPath[i]);
        }
    }
}

/// @brief         Tests if the Mediator sets and gets the DataCollectionSetting correctly
/// @param p_bool1 First bool
/// @param p_bool2 Second bool
/// @param p_bool3 Third bool
/// @param p_bool4 Fourth bool
/// @param p_bool5 Fifth bool
void TestBoolArrMediator(bool p_bool1, bool p_bool2, bool p_bool3, bool p_bool4, bool p_bool5)
{
    MockMediator2::ClearInstance();
    ASSERT_TRUE(SetupSingletonsFolder());
    tDataToStore arr = {p_bool1, p_bool2, p_bool3, p_bool4, p_bool5};
    MockMediator2::GetInstance()->SetDataCollectionSettings(arr);
    const SDAConfig config = MockMediator2::GetInstance()->GetDecisionMaker()->Config;
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
