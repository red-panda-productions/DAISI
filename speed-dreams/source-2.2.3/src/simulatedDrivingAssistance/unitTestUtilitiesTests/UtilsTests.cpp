#include <gtest/gtest.h>
#include "TestUtils.h"
#include <string>
#include <gtest/gtest-spi.h>
#include <sstream>

/// @brief Tests if message equal works
TEST(UtilsTests, MessageEqual)
{
    TestStringEqual("hello", "hello", 5);
    EXPECT_FATAL_FAILURE(TestStringEqual("hi", "hello", 2), "msg2");
    EXPECT_FATAL_FAILURE(TestStringEqual("hello", "hi", 5), "msg2");
    EXPECT_FATAL_FAILURE(TestStringEqual("hello", "hellO", 5), "Characters");
}

/// @brief Tests if Generate random char array works (kind of)
TEST(UtilsTests, RandomCharArray)
{
    char buffer[256] = {0};
    char buffer2[256] = {0};
    for (int i = 0; i < 100; i++)
    {
        int r = rand() % 255;
        GenerateRandomCharArray(buffer, r);
        ASSERT_TRUE(strlen(buffer) == r);
        bool equal = true;
        for (int j = 0; j < 256; j++)
        {
            equal = equal && buffer[j] == buffer2[j];
        }
        if (equal)
        {
            TestStringEqual(buffer, buffer2, r);  //@NOCOVERAGE, the chance is astronomically small, but can happen
        }
        for (int j = 0; j < 256; j++)
        {
            buffer2[j] = buffer[j];
        }
    }
}

/// @brief Tests whether the contains on active indicator data works correctly.
TEST(UtilsTests, ActiveIndicators)
{
    std::vector<tIndicatorData> mockData = {
        {INTERVENTION_ACTION_STEER_NONE, INTERVENTION_ACTION_TYPE_STEER, nullptr, nullptr, nullptr},
        {INTERVENTION_ACTION_SPEED_BRAKE, INTERVENTION_ACTION_TYPE_SPEED, nullptr, nullptr, nullptr}};

    ASSERT_TRUE(ActiveIndicatorsContains(mockData, INTERVENTION_ACTION_STEER_NONE));
    ASSERT_FALSE(ActiveIndicatorsContains(mockData, INTERVENTION_ACTION_SPEED_ACCEL));
}

/// @brief		  Sample function
/// @param  p_x	  Sample parameter
/// @param  p_msg Sample parameter
void Sample(int p_x, const char* p_msg)
{
    SUCCEED() << p_x << " " << p_msg;
}

// example test case
TEST_CASE(UtilsTests, SampleTest, Sample, (1, "hi"))

// example combinatorial test
BEGIN_TEST_COMBINATORIAL(UtilsTests, Combinatorial2)
int arr1[3]{0, 1, 2};
const char* arr2[2]{"hi", "hello"};
END_TEST_COMBINATORIAL2(Sample, arr1, 3, arr2, 2)

// example pairwise test
TEST(UtilsTests, ExamplePairwiseTest)
{
    int xs[20];
    for (int i = 0; i < 20; i++)
    {
        xs[i] = i;
    }
    const char* msgs[5]{"bob", "alison", "carol", "daniel", "edward"};
    PairWiseTest(Sample, xs, 20, msgs, 5);
}

/// @brief Tries to run a pairwise generator, to see if it fails
TEST(UtilsTests, PairwiseRun)
{
    PairWiseTestGenerator<4> generator;
    int dimensions[4]{2, 3, 4, 5};
    std::vector<TestCaseInfo>* testCases = generator.GetTestCases(dimensions);

    SUCCEED() << testCases->size();
    delete testCases;
}

/// @brief Tries a big test to see if a limit will be reached, currently no limits were found
TEST(UtilsTests, PairwiseLimitTest)
{
    PairWiseTestGenerator<8> generator;
    int dimensions[8]{5, 5, 5, 5, 5, 5, 5, 5};
    std::vector<TestCaseInfo>* testCases = generator.GetTestCases(dimensions);

    SUCCEED() << testCases->size();
    delete testCases;
}

/// @brief			    Checks if a tuple is covered by the test cases
/// @param  p_testCases The test cases
/// @param  p_tuple	    The tuple
/// @return			    Whether the tuple is covered
bool IsTupleCovered(std::vector<TestCaseInfo>* p_testCases, FeatureTuple& p_tuple)
{
    bool covered = false;
    for (int i = 0; i < p_testCases->size(); i++)
    {
        if (p_testCases->at(i).IsTupleCovered(p_tuple))
        {
            covered = true;
            break;
        }
    }
    return covered;
}

/// @brief Tests if all tuples are covered by the test cases generated from a generator
TEST(UtilsTests, PairWiseCoverageTest)
{
    PairWiseTestGenerator<5> generator;
    int dimensions[5]{2, 3, 4, 5, 6};
    std::vector<TestCaseInfo>* testCases = generator.GetTestCases(dimensions);

    for (int d1 = 0; d1 < 3; d1++)
    {
        for (int d2 = d1 + 1; d2 < 4; d2++)
        {
            for (int f1 = 0; f1 < dimensions[d1]; f1++)
            {
                for (int f2 = 0; f2 < dimensions[d2]; f2++)
                {
                    FeatureInfo feature1(d1, f1);
                    FeatureInfo feature2(d2, f2);
                    FeatureTuple tuple(feature1, feature2);
                    ASSERT_TRUE(IsTupleCovered(testCases, tuple));
                }
            }
        }
    }
}