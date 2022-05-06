#include <gtest/gtest.h>
#include "TestUtils.h"
#include "DecisionTuple.h"

/// @brief Tests p_setFunction, p_getFunction and p_containsFunction.
/// These functions work with float so it uses ASSERT_ALMOST_EQ
#define FLOAT_SET_GET_FUNCTION_TEST(p_setFunction, p_getFunction, p_containsFunction, p_controlValue) \
    DecisionTuple tuple;                                                                              \
    tuple.p_setFunction(p_controlValue);                                                              \
    ASSERT_ALMOST_EQ(tuple.p_getFunction(), p_controlValue, 0.0001f);                                 \
    ASSERT_TRUE(tuple.p_containsFunction());

/// @brief Tests p_setFunction and p_getFunction.
/// These functions work with int and bool so it uses ASSERT_EQ
#define SET_GET_FUNCTION_TEST(p_setFunction, p_getFunction, p_containsFunction, p_controlValue) \
    DecisionTuple tuple;                                                                        \
    tuple.p_setFunction(p_controlValue);                                                        \
    ASSERT_EQ(tuple.p_getFunction(), p_controlValue);                                           \
    ASSERT_TRUE(tuple.p_containsFunction());

#define TEST_AMOUNT 10

/// @brief Tests the SetAccel, GetAccel and ContainsAccel
TEST(DecisionTupleTests, AccelTest)
{
    Random random;
    for (int i = 0; i < TEST_AMOUNT; i++)
    {
        float controlValue = random.NextFloat(0, 1);
        FLOAT_SET_GET_FUNCTION_TEST(SetAccel, GetAccel, ContainsAccel, controlValue)
    }
}

/// @brief Tests the SetBrake, GetBrake and ContainsBrake
TEST(DecisionTupleTests, BrakeTest)
{
    Random random;
    for (int i = 0; i < TEST_AMOUNT; i++)
    {
        float controlValue = random.NextFloat(0, 1);
        FLOAT_SET_GET_FUNCTION_TEST(SetBrake, GetBrake, ContainsBrake, controlValue)
    }
}

/// @brief Tests the SetSteer, GetSteer and ContainsSteer
TEST(DecisionTupleTests, SteerTest)
{
    Random random;
    for (int i = 0; i < TEST_AMOUNT; i++)
    {
        float controlValue = random.NextFloat(-1, 1);
        FLOAT_SET_GET_FUNCTION_TEST(SetSteer, GetSteer, ContainsSteer, controlValue)
    }
}

/// @brief Tests the SetGear, GetGear and ContainsGear
TEST(DecisionTupleTests, GearTest)
{
    Random random;
    for (int i = 0; i < TEST_AMOUNT; i++)
    {
        // TODO: Implement actual test when function is implemented
        SET_GET_FUNCTION_TEST(SetGear, GetGear, ContainsGear, 0)
    }
}

/// @brief Tests the SetLights, GetLights and ContainsLights
TEST(DecisionTupleTests, LightsTest)
{
    Random random;
    for (int i = 0; i < TEST_AMOUNT; i++)
    {
        // TODO: Implement actual test when function is implemented
        SET_GET_FUNCTION_TEST(SetLights, GetLights, ContainsLights, false)
    }
}

/// @brief Tests the GetActiveDecisions function
TEST(DecisionTupleTests, ActiveDecisionsTest)
{
    DecisionTuple tuple;
    Random random;

    int decisionsCount;
    auto decisions = tuple.GetActiveDecisions(decisionsCount);
    ASSERT_EQ(decisionsCount, 0);

    float controlAccel = random.NextFloat(0, 1);
    tuple.SetAccel(controlAccel);
    tuple.GetActiveDecisions(decisionsCount);
    ASSERT_EQ(decisionsCount, 1);

    auto* accelDecision = dynamic_cast<AccelDecision*>(decisions[0]);
    ASSERT_NE(accelDecision, nullptr);

    ASSERT_ALMOST_EQ(accelDecision->AccelAmount, controlAccel, 0.001f);

    float controlSteer = random.NextFloat(-1, 1);
    tuple.SetSteer(controlSteer);
    tuple.GetActiveDecisions(decisionsCount);
    ASSERT_EQ(decisionsCount, 2);

    auto* steerDecision = dynamic_cast<SteerDecision*>(decisions[0]);
    accelDecision = dynamic_cast<AccelDecision*>(decisions[1]);
    ASSERT_NE(steerDecision, nullptr);
    ASSERT_NE(accelDecision, nullptr);

    ASSERT_ALMOST_EQ(steerDecision->SteerAmount, controlSteer, 0.001f);
    ASSERT_ALMOST_EQ(accelDecision->AccelAmount, controlAccel, 0.001f);

    float controlBrake = random.NextFloat(0, 1);
    tuple.SetBrake(controlBrake);
    tuple.GetActiveDecisions(decisionsCount);
    ASSERT_EQ(decisionsCount, 3);

    auto* brakeDecision = dynamic_cast<BrakeDecision*>(decisions[0]);
    steerDecision = dynamic_cast<SteerDecision*>(decisions[1]);
    accelDecision = dynamic_cast<AccelDecision*>(decisions[2]);
    ASSERT_NE(brakeDecision, nullptr);
    ASSERT_NE(steerDecision, nullptr);
    ASSERT_NE(accelDecision, nullptr);

    ASSERT_ALMOST_EQ(brakeDecision->BrakeAmount, controlBrake, 0.001f);
    ASSERT_ALMOST_EQ(steerDecision->SteerAmount, controlSteer, 0.001f);
    ASSERT_ALMOST_EQ(accelDecision->AccelAmount, controlAccel, 0.001f);
}