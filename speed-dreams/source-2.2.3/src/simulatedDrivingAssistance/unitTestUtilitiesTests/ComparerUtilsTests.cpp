#include <gtest/gtest.h>
#include "GeneratorUtils.h"
#include "ComparerUtils.h"

/// @brief Tests whether CompareSegments will correctly validate it's input.
TEST(ComparerUtilsTests, SegmentsLongerThanSegmentCount)
{
    TestSegments testSegments = GeneratorUtils::GenerateSegments();
    EXPECT_THROW(ComparerUtils::CompareSegments(testSegments.NextSegments, testSegments.NextSegments, testSegments.NextSegmentsCount + 1, true),
                 std::invalid_argument);
    GeneratorUtils::DestroySegments(testSegments);
}

/// @brief Tests whether comparisons work correctly between default-initialized structs
TEST(ComparerUtilsTests, EmptyCompare)
{
    tCarElt car1 = {};
    tCarElt car2 = {};
    tSituation situation1 = {};
    tSituation situation2 = {};
    TestSegments testSegments1 = {};
    TestSegments testSegments2 = {};
    constexpr int segmentCountTest = 10;
    tTrackSeg* segments1 = new tTrackSeg[segmentCountTest];
    tTrackSeg* segments2 = new tTrackSeg[segmentCountTest];
    segments1[0] = {};
    segments2[0] = {};
    for (int i = 1; i < 10; i++)
    {
        segments1[i] = {};
        segments2[i] = {};
        segments1[i - 1].next = &segments1[i];
        segments2[i - 1].next = &segments2[i];
    }

    ComparerUtils::CompareSegments(testSegments1.NextSegments, testSegments2.NextSegments, testSegments1.NextSegmentsCount, true);  // Should exit before getting to any checks
    ComparerUtils::CompareCars(car1, car2, true);                                                                                   // Test value equality
    ComparerUtils::CompareCars(car1, car2, false);                                                                                  // Test pointer inequality
    ComparerUtils::CompareSituations(situation1, situation2, true);                                                                 // Test value equality
    ComparerUtils::CompareSituations(situation1, situation2, false);                                                                // Test pointer inequality
    ComparerUtils::CompareSegments(segments1, segments2, segmentCountTest, true);                                                   // Test value equality
    ComparerUtils::CompareSegments(segments1, segments2, segmentCountTest, false);                                                  // Test pointer inequality

    delete[] segments1;
    delete[] segments2;
}

/// @brief Tests whether comparisons work correctly between randomly-generated structs
TEST(ComparerUtilsTests, GeneratedCompare)
{
    TestSegments segments1 = GeneratorUtils::GenerateSegments();
    tTrackSeg* segments2 = new tTrackSeg[segments1.NextSegmentsCount];
    segments2[0] = {};
    for (int i = 1; i < segments1.NextSegmentsCount; i++)
    {
        segments2[i] = {};
        segments2[i - 1].next = &segments2[i];
    }
    segments2[segments1.NextSegmentsCount / 2].cos = segments1.NextSegments[segments1.NextSegmentsCount / 2].cos + 1;  // Guarantee difference
    tCarElt car1 = {};
    tCarElt car2 = GeneratorUtils::GenerateCar(segments1);
    car1.index = car2.index + 1;  // Guarantee difference
    tSituation situation1 = {};
    tSituation situation2 = GeneratorUtils::GenerateSituation();
    situation2.raceInfo.features = situation1.raceInfo.features + 1;  // Guaranteee difference

    INVERT_COMP_TEST(ComparerUtils::CompareSegments(segments1.NextSegments, segments2, segments1.NextSegmentsCount, true))  // Test value inequality
    INVERT_COMP_TEST(ComparerUtils::CompareCars(car1, car2, true))                                                          // Test value inequality
    INVERT_COMP_TEST(ComparerUtils::CompareSituations(situation1, situation2, true))                                        // Test value inequality

    delete[] segments2;
    GeneratorUtils::DestroySituation(situation2);
    GeneratorUtils::DestroyCar(car2);
    GeneratorUtils::DestroySegments(segments1);
}

/// @brief Tests whether comparisons work even if only one element differs
TEST(ComparerUtilsTests, AlmostEqual)
{
    TestSegments segments1 = GeneratorUtils::GenerateSegments();
    tCarElt car1 = GeneratorUtils::GenerateCar(segments1);
    tCarElt car2 = car1;
    car2.pitcmd.stopType++;
    tSituation situation1 = GeneratorUtils::GenerateSituation();
    tSituation situation2 = situation1;
    situation2.deltaTime++;
    constexpr int segmentCountTest = 10;
    tTrackSeg* segments2 = new tTrackSeg[segmentCountTest];
    tTrackSeg* segments3 = new tTrackSeg[segmentCountTest];
    segments2[0] = {};
    segments3[0] = {};
    for (int i = 1; i < 10; i++)
    {
        segments2[i] = {};
        segments3[i] = {};
        segments2[i - 1].next = &segments2[i];
        segments3[i - 1].next = &segments3[i];
    }
    segments3[6].sin = segments2[6].sin + 1;

    INVERT_COMP_TEST(ComparerUtils::CompareCars(car1, car2, true))
    INVERT_COMP_TEST(ComparerUtils::CompareSituations(situation1, situation2, true))
    INVERT_COMP_TEST(ComparerUtils::CompareSegments(segments2, segments3, segmentCountTest, true))

    delete[] segments2;
    delete[] segments3;
    GeneratorUtils::DestroySegments(segments1);
    GeneratorUtils::DestroyCar(car1);
    GeneratorUtils::DestroySituation(situation1);
}

/// @brief Tests whether comparisons work between the exact same variable
TEST(ComparerUtilsTests, CompareEmptyWithSelf)
{
    tCarElt car1 = {};
    tSituation situation1 = {};
    constexpr int segmentCountTest = 10;
    tTrackSeg* segments1 = new tTrackSeg[segmentCountTest];
    segments1[0] = {};
    for (int i = 1; i < 10; i++)
    {
        segments1[i] = {};
        segments1[i - 1].next = &segments1[i];
    }

    ComparerUtils::CompareCars(car1, car1, true);                                                    // Test value equality
    INVERT_COMP_TEST(ComparerUtils::CompareCars(car1, car1, false))                                  // Test pointer equality
    ComparerUtils::CompareSituations(situation1, situation1, true);                                  // Test value equality
    INVERT_COMP_TEST(ComparerUtils::CompareSituations(situation1, situation1, false))                // Test pointer equality
    ComparerUtils::CompareSegments(segments1, segments1, segmentCountTest, true);                    // Test value equality
    INVERT_COMP_TEST(ComparerUtils::CompareSegments(segments1, segments1, segmentCountTest, false))  // Test pointer equality

    delete[] segments1;
}

/// @brief Tests whether comparisons work between the exact same variable
TEST(ComparerUtilsTests, CompareGeneratedWithSelf)
{
    TestSegments segments1 = GeneratorUtils::GenerateSegments();
    tCarElt car1 = GeneratorUtils::GenerateCar(segments1);
    tSituation situation1 = GeneratorUtils::GenerateSituation();

    ComparerUtils::CompareCars(car1, car1, true);                                                                                         // Test value equality
    INVERT_COMP_TEST(ComparerUtils::CompareCars(car1, car1, false))                                                                       // Test pointer equality
    ComparerUtils::CompareSituations(situation1, situation1, true);                                                                       // Test value equality
    INVERT_COMP_TEST(ComparerUtils::CompareSituations(situation1, situation1, false))                                                     // Test pointer equality
    ComparerUtils::CompareSegments(segments1.NextSegments, segments1.NextSegments, segments1.NextSegmentsCount, true);                    // Test value equality
    INVERT_COMP_TEST(ComparerUtils::CompareSegments(segments1.NextSegments, segments1.NextSegments, segments1.NextSegmentsCount, false))  // Test pointer equality

    GeneratorUtils::DestroySegments(segments1);
    GeneratorUtils::DestroyCar(car1);
    GeneratorUtils::DestroySituation(situation1);
}