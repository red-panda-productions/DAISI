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

// Test Mediator
TEST(MediatorTest, GetIntervention)
{
    Mediator mediator;

    // Test if the Mediator sets and gets the interventionType correctly
    for (int i = 0; i <= (sizeof(types)/sizeof(*types)); i++)
    {
        mediator.SetInterventionType(types[i]);
        ASSERT_EQ(types[i], mediator.GetInterventionType());
    }
}

// Test Config
TEST(ConfigTest, SetGet)
{
    Config config;

    // Test if the Config sets and gets the interventionType correctly
    for (int i = 0; i <= (sizeof(types)/sizeof(*types)); i++)
    {
        config.SetInterventionType(types[i]);
        ASSERT_EQ(types[i], config.GetInterventionType());
    }
}

// Test InterventionFactory
TEST(FactoryTest, Creation)
{
    InterventionMakerNoIntervention noInterventionMaker;
    InterventionMakerIndication indicationInterventionMaker;
    InterventionMakerAskFor askForInterventionMaker;
    InterventionMakerPerformWhenNeeded whenNeededInterventionMaker;
    InterventionMakerAlwaysIntervene alwaysInterventionMaker;
    InterventionFactory factory;

    // Creates InterventionMakers of different types and casts the resulting InterventionMaker type to
    // the type it should have made, then it checks if it throws an error.      CURRENTLY NEVER THROWS ERRORS
    ASSERT_NO_THROW((InterventionMakerNoIntervention*)factory.CreateInterventionMaker(INTERVENTION_TYPE_NO_INTERVENTION));
    ASSERT_NO_THROW((InterventionMakerIndication*)factory.CreateInterventionMaker(INTERVENTION_TYPE_INDICATION));
    ASSERT_NO_THROW((InterventionMakerAskFor*)factory.CreateInterventionMaker(INTERVENTION_TYPE_ASK_FOR));
    ASSERT_NO_THROW((InterventionMakerPerformWhenNeeded*)factory.CreateInterventionMaker(INTERVENTION_TYPE_PERFORM_WHEN_NEEDED));
    ASSERT_NO_THROW((InterventionMakerAlwaysIntervene*)factory.CreateInterventionMaker(INTERVENTION_TYPE_ALWAYS_INTERVENE));
}
// Edge case
TEST(FactoryTest, Edge)
{
    InterventionFactory factory;
    ASSERT_NO_THROW((InterventionMakerNoIntervention*)factory.CreateInterventionMaker(NULL));
}