#include <gtest/gtest.h>
#include "TestUtils.h"
#include "ConfigEnums.h"
#include "InterventionFactory.h"
#include "Mediator.h"
#include "InterventionExecutorAlwaysIntervene.h"
#include "InterventionExecutorAskFor.h"
#include "InterventionExecutorIndication.h"
#include "InterventionExecutorPerformWhenNeeded.h"
#include "InterventionExecutorNoIntervention.h"

InterventionType interventionTypesMediator[5] = {INTERVENTION_TYPE_NO_SIGNALS,
                                                 INTERVENTION_TYPE_ONLY_SIGNALS,
                                                 INTERVENTION_TYPE_ASK_FOR,
                                                 INTERVENTION_TYPE_SHARED_CONTROL,
                                                 INTERVENTION_TYPE_COMPLETE_TAKEOVER };

// Test DecisionMaker
    // Nothing to test yet

/// @brief Tests if the Mediator sets and gets the interventionType correctly
TEST(MediatorTest, GetIntervention)
{
    SMediator& mediator = SMediator::GetInstance();

    for (int i = 0; i <= (sizeof(interventionTypesMediator)/sizeof(*interventionTypesMediator)); i++)
    {
        mediator.SetInterventionType(interventionTypesMediator[i]);
        ASSERT_EQ(interventionTypesMediator[i], mediator.GetInterventionType());
    }
}

/// @brief Tests if the Factory creates the the correct InterventionExecutor
TEST(FactoryTest, Creation)
{
    InterventionFactory factory;

    // Creates InterventionExecutors of different types and casts the resulting InterventionExecutor type to
    // the type it should have made, then it checks if it throws an error.
    ASSERT_NE(
        dynamic_cast<InterventionExecutorNoIntervention*>(factory.CreateInterventionExecutor(INTERVENTION_TYPE_NO_SIGNALS))
        ,nullptr);
    ASSERT_NE(
        dynamic_cast<InterventionExecutorIndication*>(factory.CreateInterventionExecutor(INTERVENTION_TYPE_ONLY_SIGNALS))
        ,nullptr);
    ASSERT_NE(
        dynamic_cast<InterventionExecutorAskFor*>(factory.CreateInterventionExecutor(INTERVENTION_TYPE_ASK_FOR))
        ,nullptr);
    ASSERT_NE(
        dynamic_cast<InterventionExecutorPerformWhenNeeded*>(factory.CreateInterventionExecutor(INTERVENTION_TYPE_SHARED_CONTROL))
        ,nullptr);
    ASSERT_NE(
        dynamic_cast<InterventionExecutorAlwaysIntervene*>(factory.CreateInterventionExecutor(INTERVENTION_TYPE_COMPLETE_TAKEOVER))
        ,nullptr);
}

/// @brief Tests if the Factory creates the InterventionExecutorNoIntervention if the input is incorrect
TEST(FactoryTest, Edge)
{
    InterventionFactory factory;

    ASSERT_NE(dynamic_cast<InterventionExecutorNoIntervention*>(factory.CreateInterventionExecutor(6)), nullptr);
}