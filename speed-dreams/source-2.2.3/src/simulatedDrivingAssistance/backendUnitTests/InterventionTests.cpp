#include <gtest/gtest.h>
#include "TestUtils.h"
#include "Config.h"
#include "ConfigEnums.h"
#include "InterventionFactory.h"
#include "Mediator.h"
#include "mocks/DecisionMakerMock.h"
#include "PointerDistributor.h"
#include "InterventionExecutorAlwaysIntervene.h"
#include "InterventionExecutorAskFor.h"
#include "InterventionExecutorIndication.h"
#include "InterventionExecutorPerformWhenNeeded.h"
#include "InterventionExecutorNoIntervention.h"

#define INTERVENTION_TYPE_AMOUNT 5

INTERVENTION_TYPE types[INTERVENTION_TYPE_AMOUNT] = { INTERVENTION_TYPE_NO_INTERVENTION,
                               INTERVENTION_TYPE_INDICATION,
                               INTERVENTION_TYPE_ASK_FOR,
                               INTERVENTION_TYPE_PERFORM_WHEN_NEEDED,
                               INTERVENTION_TYPE_ALWAYS_INTERVENE };
// Test DecisionMaker
    // Nothing to test yet


TEST(MediatorTest, GetDistributedMediator)
{
    SMediatorDistributor distributor;
    distributor.Run();

    std::this_thread::sleep_for(std::chrono::milliseconds(400)); // also specified in main

    SMediator* mediator1 = SMediator::GetInstance();
    SMediator* mediator2 = SMediator::GetInstance();
    ASSERT_EQ(mediator1, mediator2);
}

/// @brief Tests if the Mediator sets and gets the interventionType correctly
TEST(MediatorTest, GetIntervention)
{
    SMediator mediator;

    for (int i = 0; i < INTERVENTION_TYPE_AMOUNT; i++)
    {
        mediator.SetInterventionType(types[i]);
        ASSERT_EQ(types[i], mediator.GetInterventionType());
    }
}

/// @brief Tests if the Config sets and gets the interventionType correctly
TEST(ConfigTest, SetGet)
{
    Config config;

    for (int i = 0; i < INTERVENTION_TYPE_AMOUNT; i++)
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