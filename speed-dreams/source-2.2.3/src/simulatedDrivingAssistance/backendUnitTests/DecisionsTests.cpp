#include <gtest/gtest.h>
#include "TestUtils.h"
#include "ConfigEnums.h"
#include "Mediator.h"
#include "Mediator.inl"
#include "mocks/DecisionMakerMock.h"
#include "../rppUtils/Random.hpp"
#include "../rppUtils/RppUtils.hpp"
#include "IndicatorConfig.h"
#include <config.h>

/// @brief Initialize SMediator with a car which values are set to 0
void InitializeMediator()
{
    GfInit();
    GfSetDataDir(SD_DATADIR_SRC);
    SetupSingletonsFolder();

    tCarElt car;
    car.ctrl.brakeCmd = 0;
    car.ctrl.accelCmd = 0;
    car.ctrl.steer = 0;

    CarController carController;
    carController.SetCar(&car);

    carController.SetBrakeCmd(0);
    carController.SetAccelCmd(0);
    carController.SetSteerCmd(0);

    SMediator::GetInstance()->CarController = carController;
}

/// @brief Tests if all decisions do their RunInterveneCommand correctly
TEST(DecisionsTest, RunInterveneDecisions)
{
    InitializeMediator();

    Random random;

    BrakeDecision brakeDecision;
    float controlBrakeAmount = random.NextFloat(BRAKE_THRESHOLD, BRAKE_THRESHOLD + 10);
    brakeDecision.BrakeAmount = controlBrakeAmount;
    brakeDecision.RunInterveneCommands();

    std::cout << "Testing brake...";
    ASSERT_ALMOST_EQ(controlBrakeAmount, SMediator::GetInstance()->CarController.GetBrakeCmd(), 0.001f);
    std::cout << " check" << std::endl;

    AccelDecision accelDecision;
    float controlAccelAmount = random.NextFloat();
    accelDecision.AccelAmount = controlAccelAmount;
    accelDecision.RunInterveneCommands();

    std::cout << "Testing accel...";
    ASSERT_ALMOST_EQ(controlAccelAmount, SMediator::GetInstance()->CarController.GetAccelCmd(), 0.001f);
    std::cout << " check" << std::endl;

    SteerDecision steerDecision;
    float controlSteerAmount = random.NextFloat(SDA_STEERING_THRESHOLD, SDA_STEERING_THRESHOLD + 10);
    steerDecision.SteerAmount = controlSteerAmount;
    steerDecision.RunInterveneCommands();

    std::cout << "Testing steer...";
    ASSERT_ALMOST_EQ(controlSteerAmount, SMediator::GetInstance()->CarController.GetSteerCmd(), 0.001f);
    std::cout << " check" << std::endl;

    std::cout << "Checking if no value was changed that should not have been changed...";
    ASSERT_ALMOST_EQ(controlBrakeAmount, SMediator::GetInstance()->CarController.GetBrakeCmd(), 0.001f);
    ASSERT_ALMOST_EQ(controlAccelAmount, SMediator::GetInstance()->CarController.GetAccelCmd(), 0.001f);
    ASSERT_ALMOST_EQ(controlSteerAmount, SMediator::GetInstance()->CarController.GetSteerCmd(), 0.001f);
    std::cout << " check" << std::endl;
}

/// @brief Checks if the brake decision RunIndicateCommand works correctly
TEST(DecisionTests, BrakeRunIndicateTest)
{
    InitializeMediator();

    // Load indicators from XML used for assisting the human with visual/audio indicators.
    char path[PATH_BUF_SIZE];
    snprintf(path, PATH_BUF_SIZE, CONFIG_XML_DIR_FORMAT, GfDataDir());
    IndicatorConfig::GetInstance()->LoadIndicatorData(path);

    BrakeDecision brakeDecision;
    brakeDecision.BrakeAmount = 1;
    brakeDecision.RunIndicateCommands();

    auto activeIndicators = IndicatorConfig::GetInstance()->GetActiveIndicators();

    // if the break amount is above the BRAKE_THRESHOLD defined in BrakeDecision.cpp, INTERVENTION_ACTION_BRAKE indicator should be active
    ASSERT_EQ(activeIndicators.size(), 1);
    ASSERT_EQ(activeIndicators[0].Action, INTERVENTION_ACTION_BRAKE);

    brakeDecision.BrakeAmount = 0;
    brakeDecision.RunIndicateCommands();

    activeIndicators = IndicatorConfig::GetInstance()->GetActiveIndicators();

    // if the break amount is below the BRAKE_THRESHOLD defined in BrakeDecision.cpp, no indicator should have been changed
    ASSERT_EQ(activeIndicators.size(), 1);
    ASSERT_EQ(activeIndicators[0].Action, INTERVENTION_ACTION_BRAKE);
}

/// @brief Checks if the steer decision RunIndicateCommand works correctly
TEST(DecisionsTest, SteerRunIndicateTests)
{
    InitializeMediator();

    // Load indicators from XML used for assisting the human with visual/audio indicators.
    char path[PATH_BUF_SIZE];
    snprintf(path, PATH_BUF_SIZE, CONFIG_XML_DIR_FORMAT, GfDataDir());
    IndicatorConfig::GetInstance()->LoadIndicatorData(path);

    SteerDecision steerDecision;
    steerDecision.SteerAmount = -1;
    steerDecision.RunIndicateCommands();

    // TODO: Update to have multiple indicators when indicator code is updated
    auto activeIndicators = IndicatorConfig::GetInstance()->GetActiveIndicators();

    ASSERT_EQ(activeIndicators.size(), 1);
    ASSERT_EQ(activeIndicators[0].Action, INTERVENTION_ACTION_TURN_RIGHT);

    steerDecision.SteerAmount = 1;
    steerDecision.RunIndicateCommands();

    activeIndicators = IndicatorConfig::GetInstance()->GetActiveIndicators();

    ASSERT_EQ(activeIndicators.size(), 1);
    ASSERT_EQ(activeIndicators[0].Action, INTERVENTION_ACTION_TURN_LEFT);
}

/// @brief Checks if the accel decision RunIndicateCommand works correctly
TEST(DecisionsTest, AccelRunIndicateTests)
{
    //IndicatorConfig::ClearInstance();
    InitializeMediator();

    // Load indicators from XML used for assisting the human with visual/audio indicators.
    char path[PATH_BUF_SIZE];
    snprintf(path, PATH_BUF_SIZE, CONFIG_XML_DIR_FORMAT, GfDataDir());
    IndicatorConfig::GetInstance()->LoadIndicatorData(path);

    AccelDecision accelDecision;
    accelDecision.AccelAmount = 1;
    accelDecision.RunIndicateCommands();

    auto activeIndicators = IndicatorConfig::GetInstance()->GetActiveIndicators();

    // if the accelerate amount is above the ACCEL_THRESHOLD defined in AccelDecision.cpp, INTERVENTION_ACTION_BRAKE indicator should be active
    ASSERT_EQ(activeIndicators.size(), 1);
    ASSERT_EQ(activeIndicators[0].Action, INTERVENTION_ACTION_ACCELERATE);

    accelDecision.AccelAmount = 0;
    accelDecision.RunIndicateCommands();

    activeIndicators = IndicatorConfig::GetInstance()->GetActiveIndicators();

    // if the accelerate amount is below the ACCEL_THRESHOLD defined in AccelDecision.cpp, no indicator should have been changed
    ASSERT_EQ(activeIndicators.size(), 1);
    ASSERT_EQ(activeIndicators[0].Action, INTERVENTION_ACTION_ACCELERATE);

}