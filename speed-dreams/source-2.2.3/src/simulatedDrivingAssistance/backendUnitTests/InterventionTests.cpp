#include <gtest/gtest.h>
#include "TestUtils.h"
#include "Config.h"
#include "ConfigEnums.h"
#include "InterventionFactory.h"
#include "Mediator.h"
#include "InterventionExecutorAlwaysIntervene.h"
#include "InterventionExecutorAskFor.h"
#include "InterventionExecutorIndication.h"
#include "InterventionExecutorPerformWhenNeeded.h"
#include "InterventionExecutorNoIntervention.h"

INTERVENTION_TYPE types[5] = { INTERVENTION_TYPE_NO_INTERVENTION,
                               INTERVENTION_TYPE_INDICATION,
                               INTERVENTION_TYPE_ASK_FOR,
                               INTERVENTION_TYPE_PERFORM_WHEN_NEEDED,
                               INTERVENTION_TYPE_ALWAYS_INTERVENE };

// Test DecisionMaker
    // Nothing to test yet

/// @brief Tests if the Mediator sets and gets the interventionType correctly
TEST(MediatorTest, GetIntervention)
{
    SMediator& mediator = SMediator::GetInstance();

    for (int i = 0; i <= (sizeof(types)/sizeof(*types)); i++)
    {
        mediator.SetInterventionType(types[i]);
        ASSERT_EQ(types[i], mediator.GetInterventionType());
    }
}

/// @brief Tests if the Config sets and gets the interventionType correctly
TEST(ConfigTest, SetGet)
{
    Config config;

    for (int i = 0; i <= (sizeof(types)/sizeof(*types)); i++)
    {
        config.SetInterventionType(types[i]);
        ASSERT_EQ(types[i], config.GetInterventionType());
    }
}

/// @brief Tests if the Factory creates the the correct InterventionExecutor
TEST(FactoryTest, Creation)
{
    InterventionExecutorNoIntervention noInterventionExecutor;
    InterventionExecutorIndication indicationInterventionExecutor;
    InterventionExecutorAskFor askForInterventionExecutor;
    InterventionExecutorPerformWhenNeeded whenNeededInterventionExecutor;
    InterventionExecutorAlwaysIntervene alwaysInterventionExecutor;
    InterventionFactory factory;

    // Creates InterventionExecutors of different types and casts the resulting InterventionExecutor type to
    // the type it should have made, then it checks if it throws an error.
    ASSERT_NE(
        dynamic_cast<InterventionExecutorNoIntervention*>(factory.CreateInterventionExecutor(INTERVENTION_TYPE_NO_INTERVENTION))
        ,nullptr);
    ASSERT_NE(
        dynamic_cast<InterventionExecutorIndication*>(factory.CreateInterventionExecutor(INTERVENTION_TYPE_INDICATION))
        ,nullptr);
    ASSERT_NE(
        dynamic_cast<InterventionExecutorAskFor*>(factory.CreateInterventionExecutor(INTERVENTION_TYPE_ASK_FOR))
        ,nullptr);
    ASSERT_NE(
        dynamic_cast<InterventionExecutorPerformWhenNeeded*>(factory.CreateInterventionExecutor(INTERVENTION_TYPE_PERFORM_WHEN_NEEDED))
        ,nullptr);
    ASSERT_NE(
        dynamic_cast<InterventionExecutorAlwaysIntervene*>(factory.CreateInterventionExecutor(INTERVENTION_TYPE_ALWAYS_INTERVENE))
        ,nullptr);
}

/// @brief Tests if the Factory creates the InterventionExecutorNoIntervention if the input is incorrect
TEST(FactoryTest, Edge)
{
    InterventionFactory factory;

    ASSERT_NE(dynamic_cast<InterventionExecutorNoIntervention*>(factory.CreateInterventionExecutor(6)), nullptr);
}