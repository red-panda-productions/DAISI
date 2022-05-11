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

// testing fixture for decision tests
class DecisionTest : public ::testing::TestWithParam<int>
{
private:
    tCarElt* m_car;

public:
    /// @brief Initializes the mediator with a car with brake, accel, and steer values of 0
    void SetUp() override
    {
        GfInit();
        GfSetDataDir(SD_DATADIR_SRC);
        SetupSingletonsFolder();

        m_car = new tCarElt;
        m_car->ctrl.brakeCmd = 0;
        m_car->ctrl.accelCmd = 0;
        m_car->ctrl.steer = 0;

        CarController carController;
        carController.SetCar(m_car);

        carController.SetBrakeCmd(0);
        carController.SetAccelCmd(0);
        carController.SetSteerCmd(0);

        SMediator::GetInstance()->CarController = carController;

        /* Needs to be on something other than NO_SIGNALS to retrieve active indicators*/
        SMediator::GetInstance()->SetInterventionType(INTERVENTION_TYPE_ONLY_SIGNALS);
    }

    /// @brief deletes the car from the heap at the end of a test
    void TearDown() override
    {
        delete m_car;
    }
};

/// @brief Tests if all decisions do their RunInterveneCommand correctly
TEST_F(DecisionTest, RunInterveneDecisions)
{
    IndicatorConfig::ClearInstance();
    SMediator* mediator = SMediator::GetInstance();

    // Create Data Directory if not already done.
    GfSetDataDir(SD_DATADIR_SRC);

    // Test if the mediator returns the right thresholds when it reads from a xml file
    std::string dstStr("../test_data/test_thresholds_defaults.xml");
    char buf[512];
    sprintf(buf, "%s%s", GfDataDir(), dstStr.c_str());

    mediator->SetThresholdSettings(buf);
    tThreshold thresholds = mediator->GetThresholdSettings();

    Random random;
    BrakeDecision brakeDecision;

    float controlBrakeAmount = random.NextFloat(thresholds.Brake, thresholds.Brake + 10);
    brakeDecision.BrakeAmount = controlBrakeAmount;
    brakeDecision.RunInterveneCommands();

    std::cout << "Testing brake...";
    ASSERT_ALMOST_EQ(controlBrakeAmount, mediator->CarController.GetBrakeCmd(), 0.000001f);
    std::cout << " check" << std::endl;

    AccelDecision accelDecision;
    float controlAccelAmount = random.NextFloat(thresholds.Accel, thresholds.Accel + 10);
    accelDecision.AccelAmount = controlAccelAmount;
    accelDecision.RunInterveneCommands();

    std::cout << "Testing accel...";
    ASSERT_ALMOST_EQ(controlAccelAmount, mediator->CarController.GetAccelCmd(), 0.000001f);
    std::cout << " check" << std::endl;

    SteerDecision steerDecision;
    float controlSteerAmount = random.NextFloat(thresholds.Steer, thresholds.Steer + 10);
    steerDecision.SteerAmount = controlSteerAmount;
    steerDecision.RunInterveneCommands();

    std::cout << "Testing steer...";
    ASSERT_ALMOST_EQ(controlSteerAmount, mediator->CarController.GetSteerCmd(), 0.000001f);
    std::cout << " check" << std::endl;

    std::cout << "Checking if no value was changed that should not have been changed...";
    ASSERT_ALMOST_EQ(controlBrakeAmount, mediator->CarController.GetBrakeCmd(), 0.000001f);
    ASSERT_ALMOST_EQ(controlAccelAmount, mediator->CarController.GetAccelCmd(), 0.000001f);
    ASSERT_ALMOST_EQ(controlSteerAmount, mediator->CarController.GetSteerCmd(), 0.000001f);
    std::cout << " check" << std::endl;
}

/// @brief Checks if the brake decision RunIndicateCommand works correctly
TEST_P(DecisionTest, BrakeRunIndicateTest)
{
    // Load indicators from XML used for assisting the human with visual/audio indicators.
    char path[PATH_BUF_SIZE];
    snprintf(path, PATH_BUF_SIZE, CONFIG_XML_DIR_FORMAT, GfDataDir());
    IndicatorConfig::GetInstance()->LoadIndicatorData(path);

    BrakeDecision brakeDecision;
    brakeDecision.BrakeAmount = GetParam();
    brakeDecision.RunIndicateCommands();

    auto activeIndicators = IndicatorConfig::GetInstance()->GetActiveIndicators(INTERVENTION_TYPE_ONLY_SIGNALS);

    // if the break amount is above the STANDARD_THRESHOLD_BRAKE, INTERVENTION_ACTION_BRAKE indicator should be active
    if (brakeDecision.BrakeAmount >= STANDARD_THRESHOLD_BRAKE)
    {
        ASSERT_EQ(activeIndicators.size(), 1);
        ASSERT_EQ(activeIndicators[0].Action, INTERVENTION_ACTION_BRAKE);
    }
    // TODO: else
}
INSTANTIATE_TEST_CASE_P(BrakeRunIndicateTest, DecisionTest, ::testing::Values(-99, -1, 0, 1, 2, 99));

/// @brief Checks if the steer decision RunIndicateCommand works correctly
TEST_P(DecisionTest, SteerRunIndicateTests)
{
    // Load indicators from XML used for assisting the human with visual/audio indicators.
    char path[PATH_BUF_SIZE];
    snprintf(path, PATH_BUF_SIZE, CONFIG_XML_DIR_FORMAT, GfDataDir());
    IndicatorConfig::GetInstance()->LoadIndicatorData(path);

    SteerDecision steerDecision;
    steerDecision.SteerAmount = GetParam();
    steerDecision.RunIndicateCommands();

    // TODO: Update to have multiple indicators when indicator code is updated
    auto activeIndicators = IndicatorConfig::GetInstance()->GetActiveIndicators(INTERVENTION_TYPE_ONLY_SIGNALS);

    // if the steer amount is above the STANDARD_THRESHOLD_STEER, INTERVENTION_ACTION_TURN_LEFT indicator should be active
    if (steerDecision.SteerAmount >= STANDARD_THRESHOLD_STEER)
    {
        ASSERT_EQ(activeIndicators.size(), 1);
        ASSERT_EQ(activeIndicators[0].Action, INTERVENTION_ACTION_TURN_LEFT);
    }
    // if the steer amount is below the -STANDARD_THRESHOLD_STEER, INTERVENTION_ACTION_TURN_RIGHT indicator should be active
    else if (steerDecision.SteerAmount <= -STANDARD_THRESHOLD_STEER)
    {
        ASSERT_EQ(activeIndicators.size(), 1);
        ASSERT_EQ(activeIndicators[0].Action, INTERVENTION_ACTION_TURN_RIGHT);
    }
    // TODO: else
}
INSTANTIATE_TEST_CASE_P(SteerRunIndicateTests, DecisionTest, ::testing::Values(-2, -1, 0, 1, 2, 99));

/// @brief Checks if the accel decision RunIndicateCommand works correctly
TEST_P(DecisionTest, AccelRunIndicateTests)
{
    // Load indicators from XML used for assisting the human with visual/audio indicators.
    char path[PATH_BUF_SIZE];
    snprintf(path, PATH_BUF_SIZE, CONFIG_XML_DIR_FORMAT, GfDataDir());
    IndicatorConfig::GetInstance()->LoadIndicatorData(path);

    AccelDecision accelDecision;
    accelDecision.AccelAmount = GetParam();
    accelDecision.RunIndicateCommands();

    auto activeIndicators = IndicatorConfig::GetInstance()->GetActiveIndicators(INTERVENTION_TYPE_ONLY_SIGNALS);

    // if the accelerate amount is above the STANDARD_THRESHOLD_ACCEL, INTERVENTION_ACTION_ACCELERATE indicator should be active
    if (accelDecision.AccelAmount >= STANDARD_THRESHOLD_ACCEL)
    {
        ASSERT_EQ(activeIndicators.size(), 1);
        ASSERT_EQ(activeIndicators[0].Action, INTERVENTION_ACTION_ACCELERATE);
    }
    // TODO: else
}
INSTANTIATE_TEST_CASE_P(AccelRunIndicateTests, DecisionTest, ::testing::Values(-1, 0, 1, 2, 99));
