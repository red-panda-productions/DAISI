#include <gtest/gtest.h>
#include "TestUtils.h"
#include "ConfigEnums.h"
#include "Mediator.h"
#include "Mediator.inl"
#include "mocks/DecisionMakerMock.h"
#include "IndicatorConfig.h"
#include <config.h>

#define SETUP_DECISION_TEST                                                               \
    void SetUp() override                                                                 \
    {                                                                                     \
        GfInit(false);                                                                    \
        GfSetDataDir(SD_DATADIR_SRC);                                                     \
        SetupSingletonsFolder();                                                          \
                                                                                          \
        m_car = new tCarElt;                                                              \
        m_car->ctrl.brakeCmd = 0;                                                         \
        m_car->ctrl.accelCmd = 0;                                                         \
        m_car->ctrl.steer = 0;                                                            \
                                                                                          \
        CarController carController;                                                      \
        carController.SetCar(m_car);                                                      \
                                                                                          \
        carController.SetBrakeCmd(0);                                                     \
        carController.SetAccelCmd(0);                                                     \
        carController.SetSteerCmd(0);                                                     \
                                                                                          \
        SMediator::GetInstance()->CarController = carController;                          \
                                                                                          \
        /* Needs to be on something other than NO_SIGNALS to retrieve active indicators*/ \
        SMediator::GetInstance()->SetInterventionType(INTERVENTION_TYPE_ONLY_SIGNALS);    \
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

/// @brief Tests if all decisions to their RunInterveneCommand correctly
TEST_P(DecisionTestCombinatorial, RunInterveneDecisions)
{
    tAllowedActions allowedActions;
    allowedActions.Accelerate = std::get<0>(GetParam());
    allowedActions.Brake = std::get<1>(GetParam());
    allowedActions.Steer = std::get<2>(GetParam());

    Random random;
    BrakeDecision brakeDecision;

    float controlBrakeAmount = random.NextFloat(STANDARD_THRESHOLD_BRAKE, STANDARD_THRESHOLD_BRAKE + 10);
    brakeDecision.BrakeAmount = controlBrakeAmount;
    // Determine what the brake amount should be after running
    float targetBrakeAmount = allowedActions.Brake ? controlBrakeAmount : SMediator::GetInstance()->CarController.GetBrakeCmd();

    std::cout << "Testing brake...";
    brakeDecision.RunInterveneCommands(allowedActions);
    ASSERT_ALMOST_EQ(targetBrakeAmount, SMediator::GetInstance()->CarController.GetBrakeCmd(), 0.001f);
    std::cout << " check" << std::endl;

    AccelDecision accelDecision;
    float controlAccelAmount = random.NextFloat(STANDARD_THRESHOLD_ACCEL, STANDARD_THRESHOLD_ACCEL + 10);
    accelDecision.AccelAmount = controlAccelAmount;
    // Determine what the accel amount should be after running
    float targetAccelAmount = allowedActions.Accelerate ? controlAccelAmount : SMediator::GetInstance()->CarController.GetAccelCmd();

    std::cout << "Testing accel...";
    accelDecision.RunInterveneCommands(allowedActions);
    ASSERT_ALMOST_EQ(targetAccelAmount, SMediator::GetInstance()->CarController.GetAccelCmd(), 0.001f);
    std::cout << " check" << std::endl;

    SteerDecision steerDecision;
    float controlSteerAmount = random.NextFloat(STANDARD_THRESHOLD_STEER, STANDARD_THRESHOLD_STEER + 10);
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
    IndicatorConfig::GetInstance()->LoadIndicatorData(path);

    BrakeDecision brakeDecision;
    brakeDecision.BrakeAmount = GetParam();
    brakeDecision.RunIndicateCommands();

    auto activeIndicators = IndicatorConfig::GetInstance()->GetActiveIndicators();

    // if the break amount is above the STANDARD_THRESHOLD_BRAKE, INTERVENTION_ACTION_BRAKE indicator should be active
    if (brakeDecision.BrakeAmount >= STANDARD_THRESHOLD_BRAKE)
    {
        ASSERT_TRUE(ActiveIndicatorsContains(activeIndicators, INTERVENTION_ACTION_SPEED_BRAKE));
    }
    // TODO: else
}
INSTANTIATE_TEST_SUITE_P(BrakeRunIndicateTest, DecisionTest, ::testing::Values(INT_MIN, -99, -1, 0, 1, 2, 99, INT_MAX));

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
    auto activeIndicators = IndicatorConfig::GetInstance()->GetActiveIndicators();

    // if the steer amount is above the STANDARD_THRESHOLD_STEER, INTERVENTION_ACTION_TURN_LEFT indicator should be active
    if (steerDecision.SteerAmount >= STANDARD_THRESHOLD_STEER)
    {
        ASSERT_TRUE(ActiveIndicatorsContains(activeIndicators, INTERVENTION_ACTION_STEER_LEFT));
    }
    // if the steer amount is below the -STANDARD_THRESHOLD_STEER, INTERVENTION_ACTION_TURN_RIGHT indicator should be active
    else if (steerDecision.SteerAmount <= -STANDARD_THRESHOLD_STEER)
    {
        ASSERT_TRUE(ActiveIndicatorsContains(activeIndicators, INTERVENTION_ACTION_STEER_RIGHT));
    }
    // TODO: else
}
INSTANTIATE_TEST_SUITE_P(SteerRunIndicateTests, DecisionTest, ::testing::Values(INT_MIN, -99, -2, -1, 0, 1, 2, 99, INT_MAX));

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

    auto activeIndicators = IndicatorConfig::GetInstance()->GetActiveIndicators();

    // if the accelerate amount is above the STANDARD_THRESHOLD_ACCEL, INTERVENTION_ACTION_ACCELERATE indicator should be active
    if (accelDecision.AccelAmount >= STANDARD_THRESHOLD_ACCEL)
    {
        ASSERT_TRUE(ActiveIndicatorsContains(activeIndicators, INTERVENTION_ACTION_SPEED_ACCEL));
    }
    // TODO: else
}
INSTANTIATE_TEST_SUITE_P(AccelRunIndicateTests, DecisionTest, ::testing::Values(INT_MIN, -99, -1, 0, 1, 2, 99, INT_MAX));

