#include <gtest/gtest.h>
#include "GeneratorUtils.h"
#include "ComparerUtils.h"

/// @brief Tests whether CompareSegments will correctly validate it's input.
TEST(ComparerUtilsTests, SegmentsLongerThanSegmentCount)
{
    TestSegments testSegments = GeneratorUtils::GenerateSegments();
    EXPECT_THROW(ComparerUtils::CompareSegments(testSegments.NextSegments, testSegments.NextSegments, testSegments.NextSegmentsCount + 1, true), std::invalid_argument);
    GeneratorUtils::DestroySegments(testSegments);
}