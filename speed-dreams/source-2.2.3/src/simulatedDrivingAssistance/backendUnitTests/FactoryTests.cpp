/*
 * This program has been developed by students from the bachelor Computer Science at
 * Utrecht University within the Software Project course.
 * © Copyright Utrecht University (Department of Information and Computing Sciences)
 */

#include <gtest/gtest.h>
#include "TestUtils.h"
#include "InterventionFactory.h"
#include "InterventionExecutorIndication.h"
#include "InterventionExecutorPerformWhenNeeded.h"
#include "InterventionExecutorAutonomousAI.h"
#include "InterventionExecutorNoIntervention.h"
#include "InterventionExecutorAlwaysIntervene.h"

/// @brief Tests if the Factory creates the the correct InterventionExecutor
TEST(FactoryTests, Creation)
{
    InterventionFactory factory;

    // Creates InterventionExecutors of different types and casts the resulting InterventionExecutor type to
    // the type it should have made, then it checks if it throws an error.
    ASSERT_NE(
        dynamic_cast<InterventionExecutorNoIntervention*>(factory.CreateInterventionExecutor(INTERVENTION_TYPE_NO_SIGNALS)), nullptr);
    ASSERT_NE(
        dynamic_cast<InterventionExecutorIndication*>(factory.CreateInterventionExecutor(INTERVENTION_TYPE_ONLY_SIGNALS)), nullptr);
    ASSERT_NE(
        dynamic_cast<InterventionExecutorPerformWhenNeeded*>(factory.CreateInterventionExecutor(INTERVENTION_TYPE_SHARED_CONTROL)), nullptr);
    ASSERT_NE(
        dynamic_cast<InterventionExecutorAlwaysIntervene*>(factory.CreateInterventionExecutor(INTERVENTION_TYPE_COMPLETE_TAKEOVER)), nullptr);
    ASSERT_NE(
        dynamic_cast<InterventionExecutorAutonomousAI*>(factory.CreateInterventionExecutor(INTERVENTION_TYPE_AUTONOMOUS_AI)), nullptr);
}

/// @brief Tests if the Factory creates the InterventionExecutorNoIntervention if the input is incorrect
TEST(FactoryTests, Edge)
{
    InterventionFactory factory;

    ASSERT_NE(dynamic_cast<InterventionExecutorNoIntervention*>(factory.CreateInterventionExecutor(6)), nullptr);
}