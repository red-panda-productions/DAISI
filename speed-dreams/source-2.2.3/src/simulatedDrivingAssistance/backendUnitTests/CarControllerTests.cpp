#include <gtest/gtest.h>
#include "CarController.h"
#include "../rppUtils/Random.hpp"
#include "../rppUtils/RppUtils.hpp"
#include "IndicatorConfig.h"
#include "TestUtils.h"

#define SETUP_CAR                \
    tCarElt car = {};            \
    CarController carController; \
    carController.SetCar(&car)

#define TEST_CAR_CONTROLLER_PARAM(p_paramFunction)               \
    SETUP_CAR;                                                   \
    Random random;                                               \
    for (int i = 0; i < 10; i++)                                 \
    {                                                            \
        p_paramFunction(carController, random.NextFloat(-1, 1)); \
    }

void SteerTest(CarController p_carController, float p_steer)
{
    float start = p_carController.GetCar()->ctrl.steer;
    p_carController.SetSteerCmd(p_steer);
    ASSERT_EQ(p_carController.GetSteerCmd(), p_steer + start);
}

void AccelTest(CarController p_carController, float p_accel)
{
    float start = p_carController.GetCar()->ctrl.accelCmd;
    p_carController.SetAccelCmd(p_accel);
    ASSERT_EQ(p_carController.GetAccelCmd(), p_accel + start);
}

void BrakeTest(CarController p_carController, float p_brake)
{
    float start = p_carController.GetCar()->ctrl.brakeCmd;
    p_carController.SetBrakeCmd(p_brake);
    ASSERT_EQ(p_carController.GetBrakeCmd(), std::max(p_brake, start));
}

void ClutchTest(CarController p_carController, float p_clutch)
{
    p_carController.SetClutchCmd(p_clutch);
    ASSERT_EQ(p_carController.GetClutchCmd(), p_clutch);
}

void LightTest(CarController p_carController, bool p_lights)
{
    p_carController.SetLightCmd(p_lights);
    ASSERT_EQ(p_carController.GetLightCmd(), p_lights);
}

TEST(CarControllerTests, SetCarTest)
{
    SETUP_CAR;

    const tCarElt* pointer = carController.GetCar();
    ASSERT_TRUE(pointer == &car);
}

TEST(CarControllerTests, SteerTests)
{
    TEST_CAR_CONTROLLER_PARAM(SteerTest);
}

TEST(CarControllerTests, AccelTests)
{
    TEST_CAR_CONTROLLER_PARAM(AccelTest);
}

TEST(CarControllerTests, BrakeTests)
{
    TEST_CAR_CONTROLLER_PARAM(BrakeTest);
}

TEST(CarControllerTests, ClutchTests)
{
    TEST_CAR_CONTROLLER_PARAM(ClutchTest);
}

TEST(CarControllerTests, LightTests)
{
    SETUP_CAR;
    Random random;
    for (int i = 0; i < 10; i++)
    {
        LightTest(carController, random.NextBool());
    }
}

void ShowInterventionTest(InterventionAction p_action)
{
    IndicatorConfig::ClearInstance();
    ASSERT_TRUE(SetupSingletonsFolder());
    CarController carController;

    auto activeBefore = IndicatorConfig::GetInstance()->GetActiveIndicators();

    carController.ShowIntervention(INTERVENTION_ACTION_BRAKE);

    auto activeAfter = IndicatorConfig::GetInstance()->GetActiveIndicators();

    ASSERT_TRUE(activeAfter.size() - activeBefore.size() > 0);
}

TEST_CASE(CarControllerTests, ShowInterventionTestSteer, ShowInterventionTest, (INTERVENTION_ACTION_TURN_LEFT))
TEST_CASE(CarControllerTests, ShowInterventionTestSteer2, ShowInterventionTest, (INTERVENTION_ACTION_TURN_RIGHT))
TEST_CASE(CarControllerTests, ShowInterventionTestBrake, ShowInterventionTest, (INTERVENTION_ACTION_BRAKE))