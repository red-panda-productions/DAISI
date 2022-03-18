#include <gtest/gtest.h>
#include "TestUtils.h"
#include "ConfigEnums.h"
#include "Mediator.h"


/// @brief                    Tests if the Mediator sets and gets the interventionType correctly
/// @param p_interventionType The interventionType that needs to be set
void InterventionTest(INTERVENTION_TYPE p_interventionType)
{
    SMediator& mediator = SMediator::GetInstance();

    mediator.SetInterventionType(p_interventionType);
    ASSERT_EQ(p_interventionType, mediator.GetInterventionType());
}

TEST_CASE(MediatorTests, InterventionTestNoSignals, InterventionTest, (INTERVENTION_TYPE_NO_INTERVENTION))
TEST_CASE(MediatorTests, InterventionTestOnlySignals, InterventionTest, (INTERVENTION_TYPE_INDICATION))
TEST_CASE(MediatorTests, InterventionTestAskFor, InterventionTest, (INTERVENTION_TYPE_ASK_FOR))
TEST_CASE(MediatorTests, InterventionTestSharedControl, InterventionTest, (INTERVENTION_TYPE_PERFORM_WHEN_NEEDED))
TEST_CASE(MediatorTests, InterventionTestCompleteTakeover, InterventionTest, (INTERVENTION_TYPE_ALWAYS_INTERVENE))
TEST_CASE(MediatorTests, InterventionTestEdgeCase, InterventionTest, (-1))
