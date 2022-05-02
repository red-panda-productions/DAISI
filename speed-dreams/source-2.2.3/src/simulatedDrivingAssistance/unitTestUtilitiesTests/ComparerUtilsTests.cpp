#include <gtest/gtest.h>
#include "GeneratorUtils.h"
#include "ComparerUtils.h"

/// @brief Tests whether CompareSegments will correctly validate it's input.
TEST(ComparerUtilsTests, SegmentsLongerThanSegmentCount)
{
    TestSegments testSegments = GeneratorUtils::GenerateSegments();
    EXPECT_THROW(ComparerUtils::CompareSegments(testSegments.NextSegments, testSegments.NextSegments, testSegments.NextSegmentsCount + 1, true, false),
                 std::invalid_argument);
    GeneratorUtils::DestroySegments(testSegments);
}

/// @brief Tests whether comparisons work correctly between default-initialized structs
TEST(ComparerUtilsTests, EmptyCompare)
{
    tCarElt car1 = {};
    tCarElt car2 = {};
    ComparerUtils::CompareCars(car1, car2, true, false);   // Test value equality
    ComparerUtils::CompareCars(car1, car2, false, false);  // Test pointer inequality
    tSituation situation1 = {};
    tSituation situation2 = {};
    ComparerUtils::CompareSituations(situation1, situation2, true, false);   // Test value equality
    ComparerUtils::CompareSituations(situation1, situation2, false, false);  // Test pointer inequality
}

/// @brief Tests whether comparisons work correctly between randomly-generated structs
TEST(ComparerUtilsTests, GeneratedCompare)
{
}

/// @brief Tests whether comparisons work between the exact same variable
TEST(ComparerUtilsTests, CompareWithSelf)
{
    tCarElt car1 = {};
    ComparerUtils::CompareCars(car1, car1, true, false);  // Test value equality
    ComparerUtils::CompareCars(car1, car1, false, true);  // Test pointer equality
    tSituation situation1 = {};
    ComparerUtils::CompareSituations(situation1, situation1, true, false);  // Test value equality
    ComparerUtils::CompareSituations(situation1, situation1, false, true);  // Test pointer equality
}