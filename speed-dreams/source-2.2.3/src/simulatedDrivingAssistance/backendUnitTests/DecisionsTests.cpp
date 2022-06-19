#include <gtest/gtest.h>
#include "TestUtils.h"
#include "ConfigEnums.h"
#include "Mediator.h"
#include "Mediator.inl"
#include "mocks/DecisionMakerMock.h"
#include "IndicatorConfig.h"
#include <config.h>

#define SETUP_DECISION_TEST                                                                                                   \
    void SetUp() override                                                                                                     \
    {                                                                                                                         \
        GfInit(false);                                                                                                        \
        GfSetDataDir(SD_DATADIR_SRC);                                                                                         \
        SetupSingletonsFolder();                                                                                              \
                                                                                                                              \
        m_car = new tCarElt;                                                                                                  \
        m_car->ctrl.brakeCmd = 0;                                                                                             \
        m_car->ctrl.accelCmd = 0;                                                                                             \
        m_car->ctrl.steer = 0;                                                                                                \
                                                                                                                              \
        CarController carController;                                                                                          \
        carController.SetCar(m_car);                                                                                          \
                                                                                                                              \
        carController.SetBrakeCmd(0);                                                                                         \
        carController.SetAccelCmd(0);                                                                                         \
        carController.SetSteerCmd(0);                                                                                         \
                                                                                                                              \
        SMediator::GetInstance()->CarControl = carController;                                                                 \
                                                                                                                              \
        tDecisionThresholds decisionThresholds{STANDARD_THRESHOLD_ACCEL, STANDARD_THRESHOLD_BRAKE, STANDARD_THRESHOLD_STEER}; \
        SMediator::GetInstance()->SetThresholdSettings(decisionThresholds);                                                   \
        /* Needs to be on something other than NO_SIGNALS to retrieve active indicators*/                                     \
        SMediator::GetInstance()->SetInterventionType(INTERVENTION_TYPE_SHARED_CONTROL);                                      \
                                                                                                                              \
        AllowedActions allowedActions{};                                                                                      \
        allowedActions.Steer = true;                                                                                          \
        allowedActions.Brake = true;                                                                                          \
        allowedActions.Accelerate = true;                                                                                     \
        SMediator::GetInstance()->SetAllowedActions(allowedActions);                                                          \
    }

#define TEARDOWN_DECISION_TEST \
    void TearDown() override   \
    {                          \
        delete m_car;          \
    }

// testing fixture for decision tests
class DecisionTest : public ::testing::TestWithParam<float>
{
private:
    tCarElt* m_car;

public:
    /// @brief Initializes the mediator with a car with brake, accel, and steer values of 0
    SETUP_DECISION_TEST

    /// @brief deletes the car from the heap at the end of a test
    TEARDOWN_DECISION_TEST
};

// testing fixture for decision tests
class DecisionTestCombinatorial : public ::testing::TestWithParam<std::tuple<bool, bool, bool>>
{
private:
    tCarElt* m_car;

public:
    /// @brief Initializes the mediator with a car with brake, accel, and steer values of 0
    SETUP_DECISION_TEST

    /// @brief deletes the car from the heap at the end of a test
    TEARDOWN_DECISION_TEST
};

// testing fixture for decision tests
class DecisionTestCombinatorialFloat : public ::testing::TestWithParam<std::tuple<float, float>>
{
private:
    tCarElt* m_car = nullptr;

public:
    /// @brief Initializes the mediator with a car with brake, accel, and steer values of 0
    SETUP_DECISION_TEST

    /// @brief deletes the car from the heap at the end of a test
    TEARDOWN_DECISION_TEST
};

/// @brief Tests if all decisions to their RunInterveneCommand correctly
TEST_P(DecisionTestCombinatorial, RunInterveneDecisions)
{
    tAllowedActions allowedActions;
    allowedActions.Accelerate = std::get<0>(GetParam());
    allowedActions.Brake = std::get<1>(GetParam());
    allowedActions.Steer = std::get<2>(GetParam());

    Random random;
    BrakeDecision brakeDecision = {};

    float controlBrakeAmount = random.NextFloat(STANDARD_THRESHOLD_BRAKE, STANDARD_THRESHOLD_BRAKE + 10);
    brakeDecision.SetInterventionAmount(controlBrakeAmount);
    // Determine what the brake amount should be after running
    float targetBrakeAmount = allowedActions.Brake ? controlBrakeAmount : SMediator::GetInstance()->CarControl.GetBrakeCmd();

    std::cout << "Testing brake...";
    brakeDecision.RunInterveneCommands(allowedActions);
    ASSERT_ALMOST_EQ(targetBrakeAmount, SMediator::GetInstance()->CarControl.GetBrakeCmd(), 0.001f);
    std::cout << " check" << std::endl;

    AccelDecision accelDecision = {};
    float controlAccelAmount = random.NextFloat(STANDARD_THRESHOLD_ACCEL, STANDARD_THRESHOLD_ACCEL + 10);
    accelDecision.SetInterventionAmount(controlAccelAmount);
    // Determine what the accel amount should be after running
    float targetAccelAmount = allowedActions.Accelerate ? controlAccelAmount : SMediator::GetInstance()->CarControl.GetAccelCmd();

    std::cout << "Testing accel...";
    accelDecision.RunInterveneCommands(allowedActions);
    ASSERT_ALMOST_EQ(targetAccelAmount, SMediator::GetInstance()->CarControl.GetAccelCmd(), 0.001f);
    std::cout << " check" << std::endl;

    SteerDecision steerDecision = {};
    float controlSteerAmount = random.NextFloat(STANDARD_THRESHOLD_STEER, STANDARD_THRESHOLD_STEER + 10);
    steerDecision.SetInterventionAmount(controlSteerAmount);
    // Determine what the steer amount should be after running
    float targetSteerAmount = allowedActions.Steer ? controlSteerAmount : SMediator::GetInstance()->CarControl.GetSteerCmd();

    std::cout << "Testing steer...";
    steerDecision.RunInterveneCommands(allowedActions);
    ASSERT_ALMOST_EQ(targetSteerAmount, SMediator::GetInstance()->CarControl.GetSteerCmd(), 0.001f);
    std::cout << " check" << std::endl;

    std::cout << "Checking if no value was changed that should not have been changed...";
    ASSERT_ALMOST_EQ(targetBrakeAmount, SMediator::GetInstance()->CarControl.GetBrakeCmd(), 0.001f);
    ASSERT_ALMOST_EQ(targetAccelAmount, SMediator::GetInstance()->CarControl.GetAccelCmd(), 0.001f);
    ASSERT_ALMOST_EQ(targetSteerAmount, SMediator::GetInstance()->CarControl.GetSteerCmd(), 0.001f);
    std::cout << " check" << std::endl;
}

/// @brief Checks RunInterveneDecisions for all possible permutation
INSTANTIATE_TEST_SUITE_P(RunInterveneDecisions, DecisionTestCombinatorial,
                         ::testing::Combine(::testing::Values(true, false),
                                            ::testing::Values(true, false),
                                            ::testing::Values(true, false)));

/// @brief Checks if the brake decision RunIndicateCommand works correctly
TEST_P(DecisionTest, BrakeRunIndicateTest)
{
    // Load indicators from XML used for assisting the human with visual/audio indicators.
    char path[PATH_BUF_SIZE];
    snprintf(path, PATH_BUF_SIZE, CONFIG_XML_DIR_FORMAT, GfDataDir());
    IndicatorConfig::GetInstance()->LoadIndicatorData(path, SMediator::GetInstance()->GetInterventionType());

    tAllowedActions allowedActions;
    allowedActions.Brake = true;

    BrakeDecision brakeDecision = {};
    brakeDecision.SetInterventionAmount(GetParam());
    brakeDecision.RunInterveneCommands(allowedActions);
    brakeDecision.RunIndicateCommands();

    auto activeIndicators = IndicatorConfig::GetInstance()->GetActiveIndicators();

    // if the break amount is above the STANDARD_THRESHOLD_BRAKE, INTERVENTION_ACTION_BRAKE indicator should be active
    if (brakeDecision.GetInterventionAmount() > STANDARD_THRESHOLD_BRAKE || SMediator::GetInstance()->GetInterventionType() == INTERVENTION_TYPE_AUTONOMOUS_AI)
    {
        ASSERT_TRUE(ActiveIndicatorsContains(activeIndicators, INTERVENTION_ACTION_SPEED_BRAKE));
    }
    else
    {
        ASSERT_FALSE(ActiveIndicatorsContains(activeIndicators, INTERVENTION_ACTION_SPEED_BRAKE));
    }
}
INSTANTIATE_TEST_SUITE_P(BrakeRunIndicateTest, DecisionTest,
                         ::testing::Values(INT_MIN, -99, -2, -1, -STANDARD_THRESHOLD_BRAKE, 0, STANDARD_THRESHOLD_BRAKE, 1, 2, 99, INT_MAX));

/// @brief Checks if the steer decision RunIndicateCommand works correctly
TEST_P(DecisionTest, SteerRunIndicateTests)
{
    SMediator::GetInstance()->SetInterventionType(INTERVENTION_TYPE_AUTONOMOUS_AI);

    // Load indicators from XML used for assisting the human with visual/audio indicators.
    char path[PATH_BUF_SIZE];
    snprintf(path, PATH_BUF_SIZE, CONFIG_XML_DIR_FORMAT, GfDataDir());
    IndicatorConfig::GetInstance()->LoadIndicatorData(path, SMediator::GetInstance()->GetInterventionType());

    tAllowedActions allowedActions;
    allowedActions.Steer = true;

    SteerDecision steerDecision = {};
    steerDecision.SetInterventionAmount(GetParam());
    steerDecision.RunInterveneCommands(allowedActions);
    steerDecision.RunIndicateCommands();

    auto activeIndicators = IndicatorConfig::GetInstance()->GetActiveIndicators();

    // if the steer amount is above the STANDARD_THRESHOLD_STEER, INTERVENTION_ACTION_TURN_LEFT indicator should be active
    if (steerDecision.GetInterventionAmount() > STANDARD_THRESHOLD_STEER)
    {
        ASSERT_TRUE(ActiveIndicatorsContains(activeIndicators, INTERVENTION_ACTION_STEER_LEFT));
    }
    // if the steer amount is below the -STANDARD_THRESHOLD_STEER, INTERVENTION_ACTION_TURN_RIGHT indicator should be active
    else if (steerDecision.GetInterventionAmount() < -STANDARD_THRESHOLD_STEER)
    {
        ASSERT_TRUE(ActiveIndicatorsContains(activeIndicators, INTERVENTION_ACTION_STEER_RIGHT));
    }
    // if the intervention type is autonomous AI the indicator should be active
    else if (SMediator::GetInstance()->GetInterventionType() == INTERVENTION_TYPE_AUTONOMOUS_AI)
    {
        ASSERT_TRUE(ActiveIndicatorsContains(activeIndicators, INTERVENTION_ACTION_STEER_STRAIGHT));
    }
}
INSTANTIATE_TEST_SUITE_P(SteerRunIndicateTests, DecisionTest,
                         ::testing::Values(INT_MIN, -99, -2, -1, -STANDARD_THRESHOLD_STEER, 0, STANDARD_THRESHOLD_STEER, 1, 2, 99, INT_MAX));

/// @brief Checks if the accel decision RunIndicateCommand works correctly
TEST_P(DecisionTest, AccelRunIndicateTests)
{
    // Load indicators from XML used for assisting the human with visual/audio indicators.
    char path[PATH_BUF_SIZE];
    snprintf(path, PATH_BUF_SIZE, CONFIG_XML_DIR_FORMAT, GfDataDir());
    IndicatorConfig::GetInstance()->LoadIndicatorData(path, SMediator::GetInstance()->GetInterventionType());

    tAllowedActions allowedActions;
    allowedActions.Accelerate = true;

    AccelDecision accelDecision = {};
    accelDecision.SetInterventionAmount(GetParam());
    accelDecision.RunInterveneCommands(allowedActions);
    accelDecision.RunIndicateCommands();

    auto activeIndicators = IndicatorConfig::GetInstance()->GetActiveIndicators();

    // if the accelerate amount is above the STANDARD_THRESHOLD_ACCEL, INTERVENTION_ACTION_ACCELERATE indicator should be active
    if (accelDecision.GetInterventionAmount() > STANDARD_THRESHOLD_ACCEL || SMediator::GetInstance()->GetInterventionType() == INTERVENTION_TYPE_AUTONOMOUS_AI)
    {
        ASSERT_TRUE(ActiveIndicatorsContains(activeIndicators, INTERVENTION_ACTION_SPEED_ACCEL));
    }
    else
    {
        ASSERT_FALSE(ActiveIndicatorsContains(activeIndicators, INTERVENTION_ACTION_SPEED_ACCEL));
    }
}
INSTANTIATE_TEST_SUITE_P(AccelRunIndicateTests, DecisionTest,
                         ::testing::Values(INT_MIN, -99, -1, STANDARD_THRESHOLD_ACCEL, 0, STANDARD_THRESHOLD_ACCEL, 1, 2, 99, INT_MAX));

TEST_P(DecisionTestCombinatorialFloat, MultipleIndicatorTest)
{
    SMediator::GetInstance()->SetInterventionType(INTERVENTION_TYPE_SHARED_CONTROL);

    // Load indicators from XML used for assisting the human with visual/audio indicators.
    char path[PATH_BUF_SIZE];
    snprintf(path, PATH_BUF_SIZE, CONFIG_XML_DIR_FORMAT, GfDataDir());
    IndicatorConfig::GetInstance()->LoadIndicatorData(path, SMediator::GetInstance()->GetInterventionType());

    tAllowedActions allowedActions;
    allowedActions.Steer = true;
    allowedActions.Accelerate = true;

    AccelDecision accelDecision = {};
    accelDecision.SetInterventionAmount(std::get<0>(GetParam()));
    accelDecision.RunInterveneCommands(allowedActions);
    accelDecision.RunIndicateCommands();

    SteerDecision steerDecision = {};
    steerDecision.SetInterventionAmount(std::get<1>(GetParam()));
    steerDecision.RunInterveneCommands(allowedActions);
    steerDecision.RunIndicateCommands();

    auto activeIndicators = IndicatorConfig::GetInstance()->GetActiveIndicators();

    // if the accelerate amount is above the STANDARD_THRESHOLD_ACCEL, INTERVENTION_ACTION_ACCELERATE indicator should be active
    if (accelDecision.GetInterventionAmount() > STANDARD_THRESHOLD_ACCEL || SMediator::GetInstance()->GetInterventionType() == INTERVENTION_TYPE_AUTONOMOUS_AI)
    {
        ASSERT_TRUE(ActiveIndicatorsContains(activeIndicators, INTERVENTION_ACTION_SPEED_ACCEL));
    }
    else
    {
        ASSERT_FALSE(ActiveIndicatorsContains(activeIndicators, INTERVENTION_ACTION_SPEED_ACCEL));
    }

    // if the steer amount is above the STANDARD_THRESHOLD_STEER, INTERVENTION_ACTION_TURN_LEFT indicator should be active
    if (steerDecision.GetInterventionAmount() > STANDARD_THRESHOLD_STEER)
    {
        ASSERT_TRUE(ActiveIndicatorsContains(activeIndicators, INTERVENTION_ACTION_STEER_LEFT));
    }
    // if the steer amount is below the -STANDARD_THRESHOLD_STEER, INTERVENTION_ACTION_TURN_RIGHT indicator should be active
    else if (steerDecision.GetInterventionAmount() < -STANDARD_THRESHOLD_STEER)
    {
        ASSERT_TRUE(ActiveIndicatorsContains(activeIndicators, INTERVENTION_ACTION_STEER_RIGHT));
    }
}

INSTANTIATE_TEST_SUITE_P(MultipleIndicatorTest, DecisionTestCombinatorialFloat,
                         ::testing::Combine(::testing::Values(INT_MIN, -1, STANDARD_THRESHOLD_ACCEL, 0, STANDARD_THRESHOLD_ACCEL, 1, INT_MAX),
                                            ::testing::Values(INT_MIN, -1, -STANDARD_THRESHOLD_STEER, 0, STANDARD_THRESHOLD_STEER, 1, INT_MAX)));
