#include <gtest/gtest.h>
#include "Utils.h"
#include "Random.hpp"
#include "SDAConfig.h"
#include "ConfigEnums.h"


INTERVENTION_TYPE interventionTypesConfig[5] = { INTERVENTION_TYPE_NO_INTERVENTION,
                                                 INTERVENTION_TYPE_INDICATION,
                                                 INTERVENTION_TYPE_ASK_FOR,
                                                 INTERVENTION_TYPE_PERFORM_WHEN_NEEDED,
                                                 INTERVENTION_TYPE_ALWAYS_INTERVENE };

/// @brief Tests if the SDAConfig sets and gets the interventionType correctly
TEST(ConfigTest, InterventionTypeTest)
{
    SDAConfig config;

    for (int i = 0; i <= (sizeof(interventionTypesConfig)/sizeof(*interventionTypesConfig)); i++)
    {
        config.SetInterventionType(interventionTypesConfig[i]);
        ASSERT_EQ(interventionTypesConfig[i], config.GetInterventionType());
    }
}

TEST(ConfigTest, DataCollectionTest)
{
    // Test here
}
