#include <gtest/gtest.h>
#include "Utils.h"
#include "Random.hpp"
#include "Config.h"
#include "ConfigEnums.h"

/*
INTERVENTION_TYPE types[5] = { INTERVENTION_TYPE_NO_INTERVENTION,
                               INTERVENTION_TYPE_INDICATION,
                               INTERVENTION_TYPE_ASK_FOR,
                               INTERVENTION_TYPE_PERFORM_WHEN_NEEDED,
                               INTERVENTION_TYPE_ALWAYS_INTERVENE };

/// @brief Tests if the Config sets and gets the interventionType correctly
TEST(ConfigTest, SetGet)
{
    Config config;

    for (int i = 0; i <= (sizeof(types)/sizeof(*types)); i++)
    {
        config.SetInterventionType(types[i]);
        ASSERT_EQ(types[i], config.GetInterventionType());
    }
}*/
