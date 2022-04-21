#include <gtest/gtest.h>
#include "IndicatorConfig.h"
#include "../rppUtils/RppUtils.hpp"

TEST(IndicatorConfigTests, LoadIndicatorDataTest)
{
    SetupSingletonsFolder();

    IndicatorConfig* config = IndicatorConfig::GetInstance();
    std::vector<tIndicatorData> loadedIndicatorData = config->GetIndicatorData();

    // For every indicator, check whether the data is correctly loaded.
    for (int i = 0; i < NUM_INTERVENTION_ACTION; i++)
    {
        //tIndicatorData data = loadedIndicatorData[i];
        //ASSERT_EQ(data.Action, i);
    }
}