#include <gtest/gtest.h>
#include "TestUtils.h"
#include "DecisionTuple.h"

#define FLOAT_SET_GET_FUNCTION_TEST(p_setFunction, p_getFunction, p_containsFunction, p_controlValue) \
    DecisionTuple tuple;                                                                              \
    tuple.p_setFunction(p_controlValue);                                                              \
    ASSERT_ALMOST_EQ(tuple.p_getFunction(), p_controlValue, 0.0001f);                                 \
    ASSERT_TRUE(tuple.p_containsFunction());

#define SET_GET_FUNCTION_TEST(p_setFunction, p_getFunction, p_containsFunction, p_controlValue) \
    DecisionTuple tuple;                                                                        \
    tuple.p_setFunction(p_controlValue);                                                        \
    ASSERT_EQ(tuple.p_getFunction(), p_controlValue);                                           \
    ASSERT_TRUE(tuple.p_containsFunction());

TEST(DecisionTupleTests, AccelTest)
{
    Random random;
    for (int i = 0; i < 10; i++)
    {
        float controlValue = random.NextFloat(0, 1);
        FLOAT_SET_GET_FUNCTION_TEST(SetAccel, GetAccel, ContainsAccel, controlValue)
    }
}

TEST(DecisionTupleTests, BrakeTest)
{
    Random random;
    for (int i = 0; i < 10; i++)
    {
        float controlValue = random.NextFloat(0, 1);
        FLOAT_SET_GET_FUNCTION_TEST(SetBrake, GetBrake, ContainsBrake, controlValue)
    }
}

TEST(DecisionTupleTests, SteerTest)
{
    Random random;
    for (int i = 0; i < 10; i++)
    {
        float controlValue = random.NextFloat(-1, 1);
        FLOAT_SET_GET_FUNCTION_TEST(SetSteer, GetSteer, ContainsSteer, controlValue)
    }
}

TEST(DecisionTupleTests, GearTest)
{
    Random random;
    for (int i = 0; i < 10; i++)
    {
        // TODO: Implement actual test when function is implemented
        SET_GET_FUNCTION_TEST(SetGear, GetGear, ContainsGear, 0)
    }
}

TEST(DecisionTupleTests, LightsTest)
{
    Random random;
    for (int i = 0; i < 10; i++)
    {
        // TODO: Implement actual test when function is implemented
        SET_GET_FUNCTION_TEST(SetLights, GetLights, ContainsLights, false)
    }
}

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

    dynamic_cast<AccelDecision*>(decisions[0])->AccelAmount = controlAccel;

    float controlBrake = random.NextFloat(0, 1);
    tuple.SetBrake(controlBrake);
    tuple.GetActiveDecisions(decisionsCount);
    ASSERT_EQ(decisionsCount, 2);

    dynamic_cast<BrakeDecision*>(decisions[0])->BrakeAmount = controlBrake;
    dynamic_cast<AccelDecision*>(decisions[1])->AccelAmount = controlAccel;

    float controlSteer = random.NextFloat(-1, 1);
    tuple.SetSteer(controlSteer);
    tuple.GetActiveDecisions(decisionsCount);
    ASSERT_EQ(decisionsCount, 3);

    dynamic_cast<BrakeDecision*>(decisions[0])->BrakeAmount = controlBrake;
    dynamic_cast<SteerDecision*>(decisions[1])->SteerAmount = controlSteer;
    dynamic_cast<AccelDecision*>(decisions[2])->AccelAmount = controlAccel;
}