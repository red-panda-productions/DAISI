#include <gtest/gtest.h>
#include "GeneratorUtils.h"
#include "ComparerUtils.h"

/// @brief Tests whether CompareSegments will correctly validate it's input.
TEST(ComparerUtilsTests, SegmentsLongerThanSegmentCount)
{
    TestSegments testSegments = Generator::GenerateSegments();
    EXPECT_THROW(Comparer::CompareSegments(testSegments.NextSegments, testSegments.NextSegments, testSegments.NextSegmentsCount + 1, true), std::invalid_argument);
    Generator::DestroySegments(testSegments);
}