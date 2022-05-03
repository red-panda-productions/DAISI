#include <gtest/gtest.h>
#include "BlackBoxData.h"
#include "robot.h"
#include "../rppUtils/Random.hpp"
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
        TestSegments = GeneratorUtils::GenerateSegments();
        Car = GeneratorUtils::GenerateCar(TestSegments);
        Situation = GeneratorUtils::GenerateSituation();
        TickCount = random.NextInt();
        Segments = new tTrackSeg[TestSegments.NextSegmentsCount];
    }

    TestSegments TestSegments = {};
    tTrackSeg* Segments = nullptr;
    tCarElt Car = {};
    tSituation Situation = {};
    unsigned long TickCount = 0;

    void TearDown() override
    {
        GeneratorUtils::DestroySituation(Situation);
        GeneratorUtils::DestroyCar(Car);
        GeneratorUtils::DestroySegments(TestSegments);
        delete[] Segments;
    }
};

/// @brief          Tests whether elements are the correct value or pointer
/// @param p_eqOrNe true: test for equality of values,
///                 false: test for inequality of pointers within the structs (that we don't have two stucture elements that map to the same memory location)
TEST_P(BlackBoxDataTestFixture, ElementCompareTests)
{
    bool p_eqOrNe = GetParam();
    BlackBoxData data(&Car, &Situation, TickCount, Segments, TestSegments.NextSegmentsCount);

    // Compare tickCount
    if (p_eqOrNe)
    {
        EXPECT_EQ(TickCount, data.TickCount);
    }
    else
    {
        EXPECT_NE(&TickCount, &data.TickCount);
    }

    // Compare car
    ComparerUtils::CompareCars(Car, data.Car, p_eqOrNe);
    if (p_eqOrNe)
    {
        EXPECT_EQ(Segments, data.Car.pub.trkPos.seg);
    }
    else
    {
        EXPECT_NE(&Segments, &data.Car.pub.trkPos.seg);
    }

    // Compare situation
    ComparerUtils::CompareSituations(Situation, data.Situation, p_eqOrNe);

    // Compare car.pub.trkPos.seg vs segments
    ComparerUtils::CompareSegments(Car.pub.trkPos.seg, Segments, TestSegments.NextSegmentsCount, p_eqOrNe);
}

INSTANTIATE_TEST_SUITE_P(BlackBoxDataTests, BlackBoxDataTestFixture, ::testing::Values(true, false));

/// @brief Tests whether elements that are pointers have actually been copied into a new pointer
TEST_F(BlackBoxDataTestFixture, PointerInequalityTest)
{
    BlackBoxData data(&Car, &Situation, TickCount, Segments, TestSegments.NextSegmentsCount);

    EXPECT_NE(Car.pub.trkPos.seg, data.Car.pub.trkPos.seg);
    if (Car.pub.trkPos.seg && Segments)
    {
        tTrackSeg* segOrig = &Car.pub.trkPos.seg[0];
        tTrackSeg* segCopy = &Segments[0];
        for (int i = 0; i < TestSegments.NextSegmentsCount; i++)
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
    ASSERT_THROW(BlackBoxData(nullptr, &Situation, TickCount, Segments, TestSegments.NextSegmentsCount), std::invalid_argument);
    ASSERT_THROW(BlackBoxData(&Car, nullptr, TickCount, Segments, TestSegments.NextSegmentsCount), std::invalid_argument);
    ASSERT_THROW(BlackBoxData(&Car, &Situation, TickCount, Segments, -TestSegments.NextSegmentsCount), std::invalid_argument);
}