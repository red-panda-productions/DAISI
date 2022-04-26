#include <gtest/gtest.h>
#include "TestUtils.h"
#include "DecisionTuple.h"
#include "tgf.h"
#include "SDAConfig.h"
#include "mocks/DecisionMakerMock.h"
#include "../rppUtils/RppUtils.hpp"
#include "mocks/DecisionMock.h"


/// @brief                      framework for a test that checks if a mode get run
/// @param p_INTERVENTION_TYPE  parameter that determines the mode.
void InterventionExecutorTest(unsigned int p_INTERVENTION_TYPE)
{
    DecisionMock dmock; //mock of a decision
    Random random;
    int decisionCount = random.NextInt(1, 10); // choses a random value of amount of decisions the decisionMaker gets sent

    // setting up the config
    SetupSingletonsFolder(); 
    GfInit(true);
    SDAConfig config;

    InterventionExecutor* intervention = config.SetInterventionType(p_INTERVENTION_TYPE); //determines the interventiontype that will be run

    //fill an array with the same reference of length decisionCount (used to check if it runs through all decisions correctly)
    IDecision** decisionmocks = new IDecision*[decisionCount];
    for (int i = 0; i < decisionCount; i++) { decisionmocks[i] = &dmock; }

    //Run the RunDecision code of the interventiontype.
    intervention->RunDecision(decisionmocks, decisionCount);

    // INTERVENTION_TYPE_NO_SIGNALS will not send indicators or interventions so both will return 0
    if (p_INTERVENTION_TYPE == INTERVENTION_TYPE_NO_SIGNALS) decisionCount = 0;
    ASSERT_EQ(dmock.indicate, decisionCount);

    // INTERVENTION_TYPE_ONLY_SIGNALS will not send interventions so that will be 0.
    if (p_INTERVENTION_TYPE == INTERVENTION_TYPE_ONLY_SIGNALS) decisionCount = 0; 
    ASSERT_EQ(dmock.intervene, decisionCount);
}

TEST_CASE(ExecutorTest, NoIntervention, InterventionExecutorTest, (INTERVENTION_TYPE_NO_SIGNALS)); //tests no signals
TEST_CASE(ExecutorTest, OnlySingnals, InterventionExecutorTest, (INTERVENTION_TYPE_ONLY_SIGNALS)); //tests only signals
TEST_CASE(ExecutorTest, WhenNeeded, InterventionExecutorTest, (INTERVENTION_TYPE_SHARED_CONTROL)); //test only when needed
TEST_CASE(ExecutorTest, AlwaysIntervene, InterventionExecutorTest, (INTERVENTION_TYPE_COMPLETE_TAKEOVER)); //tests always