#include <gtest/gtest.h>
#include "TestUtils.h"
#include "tgf.h"
#include "SDAConfig.h"
#include "RppUtils.hpp"
#include "mocks/DecisionMock.h"
#include "IndicatorConfig.h"
#include "Mediator.h"
#include "Mediator.inl"
#include "SDAConfig.h"
#include "mocks/DecisionMakerMock.h"

/// @brief A mediator that uses the standard SDecisionMakerMock
#define MockMediator Mediator<SDecisionMakerMock>

/// @brief A mediator that uses SDAConfig in DecisionmakerMock internally
#define SDAConfigMediator Mediator<DecisionMakerMock<SDAConfig>>
/// @brief                      framework for a test that checks if a mode get run
/// @param p_interventionType  parameter that determines the mode.
void InterventionExecutorTest(unsigned int p_interventionType)
{
    DecisionMock dmock = {};  // mock of a decision
    Random random;
    int decisionCount = random.NextInt(1, 10);  // choses a random value of amount of decisions the decisionMaker gets sent

    SDAConfigMediator::ClearInstance();

    // setting up the config
    ASSERT_TRUE(SetupSingletonsFolder());
    GfInit(GF_LOGGING_DISABLE);

    // Load indicators from XML used for assisting the human with visual/audio indicators.
    char path[PATH_BUF_SIZE];
    snprintf(path, PATH_BUF_SIZE, CONFIG_XML_DIR_FORMAT, GfDataDir());
    IndicatorConfig::GetInstance()->LoadIndicatorData(path, INTERVENTION_TYPE_SHARED_CONTROL);

    SDAConfig config;

    SMediator::GetInstance()->SetInterventionType(p_interventionType);
    InterventionExecutor* intervention = config.SetInterventionType(p_interventionType);  // determines the interventiontype that will be run

    // fill an array with the same reference of length decisionCount (used to check if it runs through all decisions correctly)
    IDecision** decisionmocks = new IDecision*[decisionCount];
    for (int i = 0; i < decisionCount; i++) { decisionmocks[i] = &dmock; }

    // Run the RunDecision code of the interventiontype.
    intervention->RunDecision(decisionmocks, decisionCount);

    // INTERVENTION_TYPE_NO_SIGNALS will not send indicators or interventions so both will return 0
    if (p_interventionType == INTERVENTION_TYPE_NO_SIGNALS) decisionCount = 0;
    ASSERT_EQ(dmock.Indicate, decisionCount);

    // INTERVENTION_TYPE_ONLY_SIGNALS will not send interventions so that will be 0.
    if (p_interventionType == INTERVENTION_TYPE_ONLY_SIGNALS) decisionCount = 0;
    ASSERT_EQ(dmock.Intervene, decisionCount);
}

TEST_CASE(ExecutorTest, NoIntervention, InterventionExecutorTest, (INTERVENTION_TYPE_NO_SIGNALS));          // tests no signals
TEST_CASE(ExecutorTest, OnlySingnals, InterventionExecutorTest, (INTERVENTION_TYPE_ONLY_SIGNALS));          // tests only signals
TEST_CASE(ExecutorTest, WhenNeeded, InterventionExecutorTest, (INTERVENTION_TYPE_SHARED_CONTROL));          // test only when needed
TEST_CASE(ExecutorTest, AlwaysIntervene, InterventionExecutorTest, (INTERVENTION_TYPE_COMPLETE_TAKEOVER));  // test control when needed
TEST_CASE(ExecutorTest, AutonomousAI, InterventionExecutorTest, (INTERVENTION_TYPE_AUTONOMOUS_AI));         // tests always