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

    // Needs to be on something other than NO_SIGNALS to retrieve active indicators
    SMediator::GetInstance()->SetInterventionType(INTERVENTION_TYPE_ONLY_SIGNALS);
}

/// @brief         Tests if all decisions to their RunInterveneCommand correctly
/// @param p_steer Whether the steer decision is allowed to run
/// @param p_accel Whether the accel decision is allowed to run
/// @param p_brake Whether the brake decision is allowed to run
void RunInterveneDecisionsTest(bool p_steer, bool p_accel, bool p_brake)
{
    InitializeMediator();
    tAllowedActions allowedActions = {p_steer, p_accel, p_brake};

    Random random;

    BrakeDecision brakeDecision;
    float controlBrakeAmount = random.NextFloat(BRAKE_THRESHOLD, BRAKE_THRESHOLD + 10);
    brakeDecision.BrakeAmount = controlBrakeAmount;
    // Determine what the brake amount should be after running
    float targetBrakeAmount = allowedActions.Brake ? controlBrakeAmount : SMediator::GetInstance()->CarController.GetBrakeCmd();

    std::cout << "Testing brake...";
    brakeDecision.RunInterveneCommands(allowedActions);
    ASSERT_ALMOST_EQ(targetBrakeAmount, SMediator::GetInstance()->CarController.GetBrakeCmd(), 0.001f);
    std::cout << " check" << std::endl;

    AccelDecision accelDecision;
    float controlAccelAmount = random.NextFloat(ACCEL_THRESHOLD, ACCEL_THRESHOLD + 10);
    accelDecision.AccelAmount = controlAccelAmount;
    // Determine what the accel amount should be after running
    float targetAccelAmount = allowedActions.Accelerate ? controlAccelAmount : SMediator::GetInstance()->CarController.GetAccelCmd();

    std::cout << "Testing accel...";
    accelDecision.RunInterveneCommands(allowedActions);
    ASSERT_ALMOST_EQ(targetAccelAmount, SMediator::GetInstance()->CarController.GetAccelCmd(), 0.001f);
    std::cout << " check" << std::endl;

    SteerDecision steerDecision;
    float controlSteerAmount = random.NextFloat(SDA_STEERING_THRESHOLD, SDA_STEERING_THRESHOLD + 10);
    steerDecision.SteerAmount = controlSteerAmount;
    // Determine what the steer amount should be after running
    float targetSteerAmount = allowedActions.Steer ? controlSteerAmount : SMediator::GetInstance()->CarController.GetSteerCmd();

    std::cout << "Testing steer...";
    steerDecision.RunInterveneCommands(allowedActions);
    ASSERT_ALMOST_EQ(targetSteerAmount, SMediator::GetInstance()->CarController.GetSteerCmd(), 0.001f);
    std::cout << " check" << std::endl;

    std::cout << "Checking if no value was changed that should not have been changed...";
    ASSERT_ALMOST_EQ(targetBrakeAmount, SMediator::GetInstance()->CarController.GetBrakeCmd(), 0.001f);
    ASSERT_ALMOST_EQ(targetAccelAmount, SMediator::GetInstance()->CarController.GetAccelCmd(), 0.001f);
    ASSERT_ALMOST_EQ(targetSteerAmount, SMediator::GetInstance()->CarController.GetSteerCmd(), 0.001f);
    std::cout << " check" << std::endl;
}

/// @brief Checks RunInterveneDecisions for all possible permutation
BEGIN_TEST_COMBINATORIAL(DecisionTests, RunInterveneDecisions)
bool booleans[] = {true, false};
END_TEST_COMBINATORIAL3(RunInterveneDecisionsTest, booleans, 2, booleans, 2, booleans, 2);

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

    // if the break amount is above the BRAKE_THRESHOLD defined in BrakeDecision.cpp, INTERVENTION_ACTION_SPEED_BRAKE indicator should be active
    ASSERT_TRUE(ActiveIndicatorsContains(activeIndicators, INTERVENTION_ACTION_SPEED_BRAKE));

    brakeDecision.BrakeAmount = 0;
    brakeDecision.RunIndicateCommands();

    activeIndicators = IndicatorConfig::GetInstance()->GetActiveIndicators();

    // if the break amount is below the BRAKE_THRESHOLD defined in BrakeDecision.cpp, no indicator should have been changed
    ASSERT_TRUE(ActiveIndicatorsContains(activeIndicators, INTERVENTION_ACTION_SPEED_BRAKE));
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

    ASSERT_TRUE(ActiveIndicatorsContains(activeIndicators, INTERVENTION_ACTION_STEER_RIGHT));

    steerDecision.SteerAmount = 1;
    steerDecision.RunIndicateCommands();

    activeIndicators = IndicatorConfig::GetInstance()->GetActiveIndicators();

    ASSERT_TRUE(ActiveIndicatorsContains(activeIndicators, INTERVENTION_ACTION_STEER_LEFT));
}

/// @brief Checks if the accel decision RunIndicateCommand works correctly
TEST(DecisionsTest, AccelRunIndicateTests)
{
    InitializeMediator();

    // Load indicators from XML used for assisting the human with visual/audio indicators.
    char path[PATH_BUF_SIZE];
    snprintf(path, PATH_BUF_SIZE, CONFIG_XML_DIR_FORMAT, GfDataDir());
    IndicatorConfig::GetInstance()->LoadIndicatorData(path);

    AccelDecision accelDecision;
    accelDecision.AccelAmount = 1;
    accelDecision.RunIndicateCommands();

    auto activeIndicators = IndicatorConfig::GetInstance()->GetActiveIndicators();

    // if the accelerate amount is above the ACCEL_THRESHOLD defined in AccelDecision.cpp, INTERVENTION_ACTION_SPEED_ACCEL indicator should be active
    ASSERT_TRUE(ActiveIndicatorsContains(activeIndicators, INTERVENTION_ACTION_SPEED_ACCEL));


    accelDecision.AccelAmount = 0;
    accelDecision.RunIndicateCommands();

    activeIndicators = IndicatorConfig::GetInstance()->GetActiveIndicators();

    // if the accelerate amount is below the ACCEL_THRESHOLD defined in AccelDecision.cpp, no indicator should have been changed
    ASSERT_TRUE(ActiveIndicatorsContains(activeIndicators, INTERVENTION_ACTION_SPEED_ACCEL));
}