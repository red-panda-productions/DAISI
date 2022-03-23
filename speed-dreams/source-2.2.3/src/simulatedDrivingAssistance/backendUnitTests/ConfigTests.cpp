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
TEST_CASE(ConfigTests, InterventionTypeTestAskFor, InterventionTypeTest, (INTERVENTION_TYPE_ASK_FOR))
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

TEST_CASE(ConfigTests, TaskTestsNoTask, TaskTest, (TASK_NO_TASK))
TEST_CASE(ConfigTests, TaskTestsLaneKeeping, TaskTest, (TASK_LANE_KEEPING))
TEST_CASE(ConfigTests, TaskTestsSpeedControl, TaskTest, (TASK_SPEED_CONTROL))

/// @brief         Tests if the SDAConfig sets and gets the IndicatorSettings correctly
/// @param p_bool1 First bool
/// @param p_bool2 Second bool
void IndicatorTest(bool p_bool1, bool p_bool2)
{
    SDAConfig config;
    bool arr[2] = { p_bool1, p_bool2 };
    config.SetIndicatorSettings(arr);
    ASSERT_EQ(arr, config.GetIndicatorSettings());
}

/// @brief Tests the SDAConfig IndicatorSetting for every possible boolean combination
BEGIN_TEST_COMBINATORIAL(ConfigTests, IndicatorSettings)
bool booleans[] = {false,true};
END_TEST_COMBINATORIAL2(IndicatorTest,booleans,2,booleans,2)

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

/// @brief         Tests if the SDAConfig sets and gets the DataCollectionSetting correctly
/// @param p_bool1 First bool
/// @param p_bool2 Second bool
/// @param p_bool3 Third bool
/// @param p_bool4 Fourth bool
/// @param p_bool5 Fifth bool
void TestBoolArr(bool p_bool1, bool p_bool2, bool p_bool3, bool p_bool4, bool p_bool5)
{
    SDAConfig config;
    bool arr[5] = { p_bool1, p_bool2, p_bool3, p_bool4, p_bool5 };
    config.SetDataCollectionSettings(arr);
    ASSERT_EQ(arr, config.GetDataCollectionSetting());
}

/// @brief Tests the SDAConfig DataCollectionSetting for every possible boolean combination
BEGIN_TEST_COMBINATORIAL(ConfigTests, DataCollectionSettings)
bool booleans[] = {false,true};
END_TEST_COMBINATORIAL5(TestBoolArr,booleans,2,booleans,2,booleans,2,booleans,2,booleans,2)
