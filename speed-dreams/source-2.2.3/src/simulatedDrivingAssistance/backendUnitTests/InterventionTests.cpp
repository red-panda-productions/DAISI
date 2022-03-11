#include <gtest/gtest.h>
#include "Utils.h"
#include "Config.h"
#include "ConfigEnums.h"
#include "InterventionFactory.h"
#include "Mediator.h"
#include "InterventionMakerAlwaysIntervene.h"
#include "InterventionMakerAskFor.h"
#include "InterventionMakerIndication.h"
#include "InterventionMakerPerformWhenNeeded.h"
#include "InterventionMakerNoIntervention.h"

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
    Mediator& mediator = Mediator::GetInstance();

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

/// @brief Tests if the Factory creates the the correct InterventionMaker
TEST(FactoryTest, Creation)
{
    InterventionMakerNoIntervention noInterventionMaker;
    InterventionMakerIndication indicationInterventionMaker;
    InterventionMakerAskFor askForInterventionMaker;
    InterventionMakerPerformWhenNeeded whenNeededInterventionMaker;
    InterventionMakerAlwaysIntervene alwaysInterventionMaker;
    InterventionFactory factory;

    // Creates InterventionMakers of different types and casts the resulting InterventionMaker type to
    // the type it should have made, then it checks if it throws an error.
    ASSERT_NE(
        dynamic_cast<InterventionMakerNoIntervention*>(factory.CreateInterventionMaker(INTERVENTION_TYPE_NO_INTERVENTION))
        ,nullptr);
    ASSERT_NE(
        dynamic_cast<InterventionMakerIndication*>(factory.CreateInterventionMaker(INTERVENTION_TYPE_INDICATION))
        ,nullptr);
    ASSERT_NE(
        dynamic_cast<InterventionMakerAskFor*>(factory.CreateInterventionMaker(INTERVENTION_TYPE_ASK_FOR))
        ,nullptr);
    ASSERT_NE(
        dynamic_cast<InterventionMakerPerformWhenNeeded*>(factory.CreateInterventionMaker(INTERVENTION_TYPE_PERFORM_WHEN_NEEDED))
        ,nullptr);
    ASSERT_NE(
        dynamic_cast<InterventionMakerAlwaysIntervene*>(factory.CreateInterventionMaker(INTERVENTION_TYPE_ALWAYS_INTERVENE))
        ,nullptr);
}

/// @brief Tests if the Factory creates the InterventionMakerNoIntervention if the input is incorrect
TEST(FactoryTest, Edge)
{
    InterventionFactory factory;

    ASSERT_NE(dynamic_cast<InterventionMakerNoIntervention*>(factory.CreateInterventionMaker(6)), nullptr);
}