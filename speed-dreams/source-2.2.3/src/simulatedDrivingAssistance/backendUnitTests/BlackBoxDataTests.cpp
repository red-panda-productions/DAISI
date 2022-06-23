/*
 * This program has been developed by students from the bachelor Computer Science at
 * Utrecht University within the Software Project course.
 * © Copyright Utrecht University (Department of Information and Computing Sciences)
 */

#include <gtest/gtest.h>
#include "BlackBoxData.h"
#include "robot.h"
#include "Random.hpp"
#include "ComparerUtils.h"
#include "GeneratorUtils.h"

// Sadly, structs don't have an (==) operator by default. We're either typing those somewhere else and using them here,
// or just comparing them here element-wise. Either way we're going to create a long comparison

// For structure elements where the copy is not implemented, no implementation is given for the generation either,
// nor are there any checks related to that element

/// @brief A testing fixture so there is less code duplication in test cases
class BlackBoxDataTestFixture : public ::testing::Test, public testing::WithParamInterface<bool>
{
protected:
    void SetUp() override
    {
        Random random;
        TestSegs = GenerateSegments();
        Car = GenerateCar(TestSegs);
        Situation = GenerateSituation();
        TickCount = random.NextInt();
        Segments = new tTrackSeg[TestSegs.NextSegmentsCount];
    }

    TestSegments TestSegs = {};
    tTrackSeg* Segments = nullptr;
    tCarElt Car = {};
    tSituation Situation = {};
    unsigned long TickCount = 0;

    void TearDown() override
    {
        DestroySituation(Situation);
        DestroyCar(Car);
        DestroySegments(TestSegs);
        delete[] Segments;
    }
};

/// @brief                  Tests whether elements are the correct value or pointer
/// @param p_comparisonType true: test for equality of values,
///                         false: test for inequality of pointers within the structs (that we don't have two stucture elements that map to the same memory location)
TEST_P(BlackBoxDataTestFixture, ElementCompareTests)
{
    bool p_comparisonType = GetParam();

    tCar table = GenerateSimCar(Car);

    BlackBoxData data(&table, &Car, &Situation, TickCount, Segments, TestSegs.NextSegmentsCount);

    // Compare tickCount
    if (p_comparisonType)
    {
        EXPECT_EQ(TickCount, data.TickCount);
    }
    else
    {
        EXPECT_NE(&TickCount, &data.TickCount);
    }

    // Compare car
    CompareCars(Car, data.Car, p_comparisonType);
    if (p_comparisonType)
    {
        EXPECT_EQ(Segments, data.Car.pub.trkPos.seg);
    }
    else
    {
        EXPECT_NE(&Segments, &data.Car.pub.trkPos.seg);
    }

    // Compare situation
    CompareSituations(Situation, data.Situation, p_comparisonType);

    // Compare car.pub.trkPos.seg vs segments
    CompareSegments(Car.pub.trkPos.seg, Segments, TestSegs.NextSegmentsCount, p_comparisonType);
}

INSTANTIATE_TEST_SUITE_P(BlackBoxDataTests, BlackBoxDataTestFixture, ::testing::Values(true, false));

/// @brief Tests whether elements that are pointers have actually been copied into a new pointer
TEST_F(BlackBoxDataTestFixture, PointerInequalityTest)
{
    tCar table = GenerateSimCar(Car);
    BlackBoxData data(&table, &Car, &Situation, TickCount, Segments, TestSegs.NextSegmentsCount);

    EXPECT_NE(Car.pub.trkPos.seg, data.Car.pub.trkPos.seg);
    if (Car.pub.trkPos.seg && Segments)
    {
        tTrackSeg* segOrig = &Car.pub.trkPos.seg[0];
        tTrackSeg* segCopy = &Segments[0];
        for (int i = 0; i < TestSegs.NextSegmentsCount; i++)
        {
            EXPECT_NE(segOrig, segCopy);
            segOrig = (*segOrig).next;
            segCopy = (*segCopy).next;
        }
    }
}

/// @brief Tests whether BlackBoxData() indeed throws if it receives invalid input
TEST_F(BlackBoxDataTestFixture, ExceptionsThrownTest)
{
    tCar table = GenerateSimCar(Car);
    ASSERT_THROW(BlackBoxData(&table, nullptr, &Situation, TickCount, Segments, TestSegs.NextSegmentsCount), std::invalid_argument);
    ASSERT_THROW(BlackBoxData(&table, &Car, nullptr, TickCount, Segments, TestSegs.NextSegmentsCount), std::invalid_argument);
    ASSERT_THROW(BlackBoxData(&table, &Car, &Situation, TickCount, Segments, -TestSegs.NextSegmentsCount), std::invalid_argument);
    ASSERT_THROW(BlackBoxData(nullptr, &Car, &Situation, TickCount, Segments, -TestSegs.NextSegmentsCount), std::invalid_argument);
}