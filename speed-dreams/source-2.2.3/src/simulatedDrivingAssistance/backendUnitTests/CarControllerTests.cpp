#include <gtest/gtest.h>
#include "CarController.h"
#include "../rppUtils/Random.hpp"
#include "../rppUtils/RppUtils.hpp"
#include "IndicatorConfig.h"
#include "TestUtils.h"

/// @brief sets up teh car controller
#define SETUP_CAR                \
    tCarElt car = {};            \
    CarController carController; \
    carController.SetCar(&car)

/// @brief tests a controller param given a function
#define TEST_CAR_CONTROLLER_PARAM(p_paramFunction)               \
    SETUP_CAR;                                                   \
    Random random;                                               \
    for (int i = 0; i < 10; i++)                                 \
    {                                                            \
        p_paramFunction(carController, random.NextFloat(-1, 1)); \
    }

/// @brief                 Tests the steer parameter of a car controller
/// @param p_carController The car controller
/// @param p_steer         The steer value
void SteerTest(CarController p_carController, float p_steer)
{
    float start = p_carController.GetCar()->ctrl.steer;
    p_carController.SetSteerCmd(p_steer);
    ASSERT_EQ(p_carController.GetSteerCmd(), p_steer + start);
}

/// @brief                 Tests the acceleration parameter of a car controller
/// @param p_carController The car controller
/// @param p_accel         The acceleration value
void AccelTest(CarController p_carController, float p_accel)
{
    float start = p_carController.GetCar()->ctrl.accelCmd;
    p_carController.SetAccelCmd(p_accel);
    ASSERT_EQ(p_carController.GetAccelCmd(), p_accel + start);
}

/// @brief                 Tests the brake parameter of a car controller
/// @param p_carController The car controller
/// @param p_brake         The brake value
void BrakeTest(CarController p_carController, float p_brake)
{
    float start = p_carController.GetCar()->ctrl.brakeCmd;
    p_carController.SetBrakeCmd(p_brake);
    ASSERT_EQ(p_carController.GetBrakeCmd(), std::max(p_brake, start));
}

/// @brief                 Tests the clutch parameter of a car controller
/// @param p_carController The car controller
/// @param p_clutch        The clutch value
void ClutchTest(CarController p_carController, float p_clutch)
{
    p_carController.SetClutchCmd(p_clutch);
    ASSERT_EQ(p_carController.GetClutchCmd(), p_clutch);
}

/// @brief                 Tests the lights parameter of a car controller
/// @param p_carController The car controller
/// @param p_lights        The lights value
void LightTest(CarController p_carController, bool p_lights)
{
    p_carController.SetLightCmd(p_lights);
    ASSERT_EQ(p_carController.GetLightCmd(), p_lights);
}

/// @brief Tests the set car function of car controller
TEST(CarControllerTests, SetCarTest)
{
    SETUP_CAR;

    const tCarElt* pointer = carController.GetCar();
    ASSERT_TRUE(pointer == &car);
}

/// @brief Tests the steer parameter of car controller
TEST(CarControllerTests, SteerTests)
{
    TEST_CAR_CONTROLLER_PARAM(SteerTest);
}

/// @brief Tests the acceleration parameter of car controller
TEST(CarControllerTests, AccelTests)
{
    TEST_CAR_CONTROLLER_PARAM(AccelTest);
}

/// @brief Tests the brake parameter of car controller
TEST(CarControllerTests, BrakeTests)
{
    TEST_CAR_CONTROLLER_PARAM(BrakeTest);
}

/// @brief Tests the clutch parameter of car controller
TEST(CarControllerTests, ClutchTests)
{
    TEST_CAR_CONTROLLER_PARAM(ClutchTest);
}

/// @brief Tests the lights parameter of car controller
TEST(CarControllerTests, LightTests)
{
    SETUP_CAR;
    Random random;
    for (int i = 0; i < 10; i++)
    {
        LightTest(carController, random.NextBool());
    }
}

/// @brief          Tests the show intervention function of car controller
/// @param p_action The intervention action that will be set
void ShowInterventionTest(InterventionAction p_action)
{
    IndicatorConfig::ClearInstance();
    ASSERT_TRUE(SetupSingletonsFolder());
    CarController carController;

    auto activeBefore = IndicatorConfig::GetInstance()->GetActiveIndicators();

    carController.ShowIntervention(p_action);

    auto activeAfter = IndicatorConfig::GetInstance()->GetActiveIndicators();

    ASSERT_TRUE(activeAfter.size() - activeBefore.size() > 0);
}

TEST_CASE(CarControllerTests, ShowInterventionTestSteer, ShowInterventionTest, (INTERVENTION_ACTION_TURN_LEFT))
TEST_CASE(CarControllerTests, ShowInterventionTestSteer2, ShowInterventionTest, (INTERVENTION_ACTION_TURN_RIGHT))
TEST_CASE(CarControllerTests, ShowInterventionTestBrake, ShowInterventionTest, (INTERVENTION_ACTION_BRAKE))