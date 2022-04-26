#include <gtest/gtest.h>
#include "TestUtils.h"
#include "DecisionTuple.h"
#include "tgf.h"
#include "SDAConfig.h"
#include "mocks/DecisionMakerMock.h"
#include "../rppUtils/RppUtils.hpp"
#include "mocks/BrakeMock.h"
#include "mocks/AccelMock.h"
#include "mocks/SteerMock.h"

void InterventionExecutorTest(int p_expectedValueIndicate, int p_expectedValueIntervene, unsigned int p_INTERVENTION_TYPE)
{
    BrakeMock bmock;
    AccelMock amock;
    SteerMock smock;
    const int descionCount = 3;
    int indicate;
    int intervene;

    SetupSingletonsFolder();
    GfInit(true);
    SDAConfig config;
    InterventionExecutor* intervention = config.SetInterventionType(p_INTERVENTION_TYPE);
    IDecision* mockdescions[descionCount] = {&bmock, &amock, &smock};
    intervention->RunDecision(mockdescions, descionCount);
    indicate = bmock.indicate + amock.indicate + smock.indicate;
    intervene = bmock.intervene + bmock.intervene + smock.intervene;
    ASSERT_EQ(indicate, p_expectedValueIndicate);
    ASSERT_EQ(intervene, p_expectedValueIntervene);
}

TEST_CASE(ExecutorTest, NoIntervention, InterventionExecutorTest, (0, 0, INTERVENTION_TYPE_NO_SIGNALS));
TEST_CASE(ExecutorTest, OnlySingnals, InterventionExecutorTest, (3, 0, INTERVENTION_TYPE_ONLY_SIGNALS));
TEST_CASE(ExecutorTest, WhenNeeded, InterventionExecutorTest, (3, 3, INTERVENTION_TYPE_SHARED_CONTROL));
TEST_CASE(ExecutorTest, AlwaysIntervene, InterventionExecutorTest, (3, 3, INTERVENTION_TYPE_COMPLETE_TAKEOVER));