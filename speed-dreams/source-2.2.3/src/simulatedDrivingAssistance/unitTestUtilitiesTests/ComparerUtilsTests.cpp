#include <gtest/gtest.h>
#include "GeneratorUtils.h"
#include "ComparerUtils.h"

/// @brief Tests whether CompareSegments will correctly validate it's input.
TEST(ComparerUtilsTests, SegmentsLongerThanSegmentCount)
{
    TestSegments testSegments = GenerateSegments();
    EXPECT_THROW(CompareSegments(testSegments.NextSegments, testSegments.NextSegments, testSegments.NextSegmentsCount + 1, true),
                 std::invalid_argument);
    DestroySegments(testSegments);
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
#define SEGMENT_COUNT_TEST 10
    tTrackSeg segments1[SEGMENT_COUNT_TEST];
    tTrackSeg segments2[SEGMENT_COUNT_TEST];
    segments1[0] = {};
    segments2[0] = {};
    for (int i = 1; i < 10; i++)
    {
        segments1[i] = {};
        segments2[i] = {};
        segments1[i - 1].next = &segments1[i];
        segments2[i - 1].next = &segments2[i];
    }

    CompareSegments(testSegments1.NextSegments, testSegments2.NextSegments, testSegments1.NextSegmentsCount, true);  // Should exit before getting to any checks
    CompareCars(car1, car2, true);                                                                                   // Test value equality
    CompareCars(car1, car2, false);                                                                                  // Test pointer inequality
    CompareSituations(situation1, situation2, true);                                                                 // Test value equality
    CompareSituations(situation1, situation2, false);                                                                // Test pointer inequality
    CompareSegments(segments1, segments2, SEGMENT_COUNT_TEST, true);                                                 // Test value equality
    CompareSegments(segments1, segments2, SEGMENT_COUNT_TEST, false);                                                // Test pointer inequality
}

/// @brief Tests whether comparisons work correctly between randomly-generated structs
TEST(ComparerUtilsTests, GeneratedCompare)
{
    TestSegments segments1 = GenerateSegments();
    tTrackSeg* segments2 = new tTrackSeg[segments1.NextSegmentsCount];
    segments2[0] = {};
    for (int i = 1; i < segments1.NextSegmentsCount; i++)
    {
        segments2[i] = {};
        segments2[i - 1].next = &segments2[i];
    }
    segments2[segments1.NextSegmentsCount / 2].cos = segments1.NextSegments[segments1.NextSegmentsCount / 2].cos + 1;  // Guarantee difference
    tCarElt car1 = {};
    tCarElt car2 = GenerateCar(segments1);
    car1.index = car2.index + 1;  // Guarantee difference
    tSituation situation1 = {};
    tSituation situation2 = GenerateSituation();
    situation2.raceInfo.features = situation1.raceInfo.features + 1;  // Guarantee difference

    INVERT_COMP_TEST(CompareSegments(segments1.NextSegments, segments2, segments1.NextSegmentsCount, true))  // Test value inequality
    INVERT_COMP_TEST(CompareCars(car1, car2, true))                                                          // Test value inequality
    INVERT_COMP_TEST(CompareSituations(situation1, situation2, true))                                        // Test value inequality

    delete[] segments2;
    DestroySituation(situation2);
    DestroyCar(car2);
    DestroySegments(segments1);
}

/// @brief Tests whether comparisons work even if only one element differs
TEST(ComparerUtilsTests, AlmostEqual)
{
    TestSegments segments1 = GenerateSegments();
    tCarElt car1 = GenerateCar(segments1);
    tCarElt car2 = car1;
    car2.pitcmd.stopType++;
    tSituation situation1 = GenerateSituation();
    tSituation situation2 = situation1;
    situation2.deltaTime++;
#define SEGMENT_COUNT_TEST 10
    tTrackSeg segments2[SEGMENT_COUNT_TEST];
    tTrackSeg segments3[SEGMENT_COUNT_TEST];
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

    INVERT_COMP_TEST(CompareCars(car1, car2, true))
    INVERT_COMP_TEST(CompareSituations(situation1, situation2, true))
    INVERT_COMP_TEST(CompareSegments(segments2, segments3, SEGMENT_COUNT_TEST, true))

    DestroySegments(segments1);
    DestroyCar(car1);
    DestroySituation(situation1);
}

/// @brief Tests whether comparisons work between the exact same variable
TEST(ComparerUtilsTests, CompareEmptyWithSelf)
{
    tCarElt car1 = {};
    tSituation situation1 = {};
#define SEGMENT_COUNT_TEST 10
    tTrackSeg segments1[SEGMENT_COUNT_TEST];
    segments1[0] = {};
    for (int i = 1; i < 10; i++)
    {
        segments1[i] = {};
        segments1[i - 1].next = &segments1[i];
    }

    CompareCars(car1, car1, true);                                                      // Test value equality
    INVERT_COMP_TEST(CompareCars(car1, car1, false))                                    // Test pointer equality
    CompareSituations(situation1, situation1, true);                                    // Test value equality
    INVERT_COMP_TEST(CompareSituations(situation1, situation1, false))                  // Test pointer equality
    CompareSegments(segments1, segments1, SEGMENT_COUNT_TEST, true);                    // Test value equality
    INVERT_COMP_TEST(CompareSegments(segments1, segments1, SEGMENT_COUNT_TEST, false))  // Test pointer equality
}

/// @brief Tests whether comparisons work between the exact same variable
TEST(ComparerUtilsTests, CompareGeneratedWithSelf)
{
    TestSegments segments1 = GenerateSegments();
    tCarElt car1 = GenerateCar(segments1);
    tSituation situation1 = GenerateSituation();

    CompareCars(car1, car1, true);                                                                                         // Test value equality
    INVERT_COMP_TEST(CompareCars(car1, car1, false))                                                                       // Test pointer equality
    CompareSituations(situation1, situation1, true);                                                                       // Test value equality
    INVERT_COMP_TEST(CompareSituations(situation1, situation1, false))                                                     // Test pointer equality
    CompareSegments(segments1.NextSegments, segments1.NextSegments, segments1.NextSegmentsCount, true);                    // Test value equality
    INVERT_COMP_TEST(CompareSegments(segments1.NextSegments, segments1.NextSegments, segments1.NextSegmentsCount, false))  // Test pointer equality

    DestroySegments(segments1);
    DestroyCar(car1);
    DestroySituation(situation1);
}