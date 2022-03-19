#include <gtest/gtest.h>
#include "TestUtils.h"
#include "SDAConfig.h"
#include "ConfigEnums.h"

#define INTERVENTION_TYPE_AMOUNT 5

InterventionType typesConfig[INTERVENTION_TYPE_AMOUNT] = { INTERVENTION_TYPE_NO_SIGNALS,
                                                           INTERVENTION_TYPE_ONLY_SIGNALS,
                                                           INTERVENTION_TYPE_ASK_FOR,
                                                           INTERVENTION_TYPE_ASK_FOR,
                                                           INTERVENTION_TYPE_COMPLETE_TAKEOVER };

/// @brief Tests if the SDAConfig sets and gets the interventionType correctly
TEST(ConfigTest, InterventionType)
{
    SDAConfig config;

    for (int i = 0; i < INTERVENTION_TYPE_AMOUNT; i++)
    {
        config.SetInterventionType(typesConfig[i]);
        ASSERT_EQ(typesConfig[i], config.GetInterventionType());
    }
}

/// @brief         Tests if the SDAConfig sets and gets the DataCollectionSetting correctly
/// @param p_bool1 First bool
/// @param p_bool2 Second bool
/// @param p_bool3 Third bool
/// @param p_bool4 Fourth bool
/// @param p_bool5 Fifth bool
void TestBoolArr(bool p_bool1, bool p_bool2, bool p_bool3, bool p_bool4, bool p_bool5)
{
    SDAConfig config;
    bool arr[5] = { p_bool1, p_bool2, p_bool3, p_bool4, p_bool5 };
    config.SetDataCollectionSettings(arr);
    ASSERT_EQ(arr, config.GetDataCollectionSetting());
}

/// @brief Tests the SDAConfig DataCollectionSetting for every possible boolean combination
BEGIN_TEST_COMBINATORIAL(ConfigTest, DataCollectionSetting)
bool booleans[] = {false,true};
END_TEST_COMBINATORIAL5(TestBoolArr,booleans,2,booleans,2,booleans,2,booleans,2,booleans,2)
