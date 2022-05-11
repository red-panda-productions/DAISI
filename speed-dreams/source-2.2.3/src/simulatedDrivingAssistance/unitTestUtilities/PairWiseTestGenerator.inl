#pragma once
#include "PairWiseTestGenerator.h"

/// @brief Creates an implementation of a pairwise test generator
#define CREATE_PAIR_WISE_TEST_GENERATOR_IMPLEMENTATION(dimensions)                                                                               \
    template std::vector<TestCaseInfo>* PairWiseTestGenerator<dimensions>::GetTestCases(int* p_dimensions);                                      \
    template void PairWiseTestGenerator<dimensions>::CreateAllTuples();                                                                          \
    template void PairWiseTestGenerator<dimensions>::CreateTuples(int p_dimension, int p_feature, Queue& p_uncoveredTuple);                      \
    template bool PairWiseTestGenerator<dimensions>::GetNextTuple(FeatureTuple& tuple);                                                          \
    template void PairWiseTestGenerator<dimensions>::CreateTestCase(FeatureTuple& p_tuple, TestCaseInfo& p_testCase);                            \
    template void PairWiseTestGenerator<dimensions>::CreateRandomTestCase(FeatureTuple& p_tuple, TestCaseInfo& p_testCase);                      \
    template int PairWiseTestGenerator<dimensions>::MaximizeCoverage(TestCaseInfo& p_testCase, FeatureTuple& p_tuple);                           \
    template int* PairWiseTestGenerator<dimensions>::GetMutableDimensions(FeatureTuple& p_tuple, int& p_count);                                  \
    template void PairWiseTestGenerator<dimensions>::ScrambleDimensions(int* p_dimensions, int p_dimensionCount);                                \
    template int PairWiseTestGenerator<dimensions>::MaximizeCoverageForDimension(TestCaseInfo& p_testCase, int p_dimension, int p_bestCoverage); \
    template int PairWiseTestGenerator<dimensions>::CountTuplesCoveredByTest(TestCaseInfo& p_testCase, int p_dimension, int p_feature);          \
    template void PairWiseTestGenerator<dimensions>::RemoveTuplesCoveredByTest(TestCaseInfo& p_testCase)

/// @brief				 Creates all of the pair-wise test cases
/// @param  p_dimensions The info of how many parameters each array has
/// @return				 The pair-wise test cases
template <int DimensionsCount>
std::vector<TestCaseInfo>* PairWiseTestGenerator<DimensionsCount>::GetTestCases(int* p_dimensions)
{
    for (int i = 0; i < DimensionsCount; i++)
    {
        m_dimensions[i] = p_dimensions[i];
    }

    CreateAllTuples();

    std::vector<TestCaseInfo>* testCases = new std::vector<TestCaseInfo>();

    while (true)
    {
        FeatureTuple tuple;

        if (!GetNextTuple(tuple)) break;

        TestCaseInfo testCase(DimensionsCount);

        CreateTestCase(tuple, testCase);

        RemoveTuplesCoveredByTest(testCase);

        testCases->push_back(testCase);
    }

    for (int i = 0; i < DimensionsCount; i++)
    {
        delete[] m_uncoveredTuples[i];
    }

    return testCases;
}

/// @brief Creates all tuples that exist between pairs
template <int DimensionsCount>
void PairWiseTestGenerator<DimensionsCount>::CreateAllTuples()
{
    for (int d = 0; d < DimensionsCount; d++)
    {
        m_uncoveredTuples[d] = new Queue[m_dimensions[d]];
        for (int f = 0; f < m_dimensions[d]; f++)
        {
            CreateTuples(d, f, m_uncoveredTuples[d][f]);
        }
    }
}

/// @brief					 Creates tuples for one dimension
/// @param  p_dimension      The dimension of the tuple
/// @param  p_feature	     The feature of the tuple
/// @param  p_uncoveredTuple The queue of uncovered tuples
template <int DimensionsCount>
void PairWiseTestGenerator<DimensionsCount>::CreateTuples(int p_dimension, int p_feature, Queue& p_uncoveredTuple)
{
    FeatureInfo f1 = FeatureInfo(p_dimension, p_feature);
    FeatureTuple t = FeatureTuple(f1);
    p_uncoveredTuple.emplace(t);
    FeatureInfo f2;
    for (int d = 0; d < DimensionsCount; d++)
    {
        if (d == p_dimension) continue;
        for (int f = 0; f < m_dimensions[d]; f++)
        {
            f2.Dimension = d;
            f2.Feature = f;
            t.SetTuple(f1, f2);
            p_uncoveredTuple.emplace(t);
        }
    }
}

/// @brief          Returns the next uncovered tuple from m_uncoveredTuples
/// @param  p_tuple The returned tuple
/// @return	        Whether the operation succeeded or failed
template <int DimensionsCount>
bool PairWiseTestGenerator<DimensionsCount>::GetNextTuple(FeatureTuple& p_tuple)
{
    for (int d = 0; d < DimensionsCount; d++)
    {
        for (int f = 0; f < m_dimensions[d]; f++)
        {
            if (m_uncoveredTuples[d][f].empty()) continue;

            p_tuple = m_uncoveredTuples[d][f].front();
            m_uncoveredTuples[d][f].pop();
            return true;
        }
    }
    return false;
}

/// @brief			   Creates a test case based on a tuple, it tries 7 times
/// @param  p_tuple	   The tuple
/// @param  p_testCase the returned test case
template <int DimensionsCount>
void PairWiseTestGenerator<DimensionsCount>::CreateTestCase(FeatureTuple& p_tuple, TestCaseInfo& p_testCase)
{
    int bestCoverage = -1;

    for (int i = 0; i < 7; i++)
    {
        TestCaseInfo testCase(DimensionsCount);
        CreateRandomTestCase(p_tuple, testCase);
        int coverage = MaximizeCoverage(testCase, p_tuple);
        if (coverage > bestCoverage)
        {
            for (int i = 0; i < testCase.Length; i++)
            {
                p_testCase.Features[i] = testCase.Features[i];
            }
            bestCoverage = coverage;
        }
    }
}

/// @brief			   Generates a random test case based on a tuple
/// @param  p_tuple    The tuple
/// @param  p_testCase The returned test case
template <int DimensionsCount>
void PairWiseTestGenerator<DimensionsCount>::CreateRandomTestCase(FeatureTuple& p_tuple, TestCaseInfo& p_testCase)
{
    for (int d = 0; d < DimensionsCount; d++)
    {
        p_testCase.Features[d] = m_random.NextInt(m_dimensions[d]);
    }

    for (int i = 0; i < p_tuple.Count(); i++)
    {
        p_testCase.Features[p_tuple[i].Dimension] = p_tuple[i].Feature;
    }
}

/// @brief			   Tries to maximize the coverage of a test case based on a tuple
/// @param  p_testCase The test case that needs to be maximized
/// @param  p_tuple    The tuple
/// @return			   The coverage amount
template <int DimensionsCount>
int PairWiseTestGenerator<DimensionsCount>::MaximizeCoverage(TestCaseInfo& p_testCase, FeatureTuple& p_tuple)
{
    int totalCoverage = 1;
    int mutableCount = 0;
    int* mutableDimensions = GetMutableDimensions(p_tuple, mutableCount);

    while (true)
    {
        bool progress = false;

        ScrambleDimensions(mutableDimensions, mutableCount);

        for (int i = 0; i < mutableCount; i++)
        {
            int d = mutableDimensions[i];
            int bestCoverage = CountTuplesCoveredByTest(p_testCase, d, p_testCase.Features[d]);
            int newCoverage = MaximizeCoverageForDimension(p_testCase, d, bestCoverage);
            totalCoverage += newCoverage;
            if (newCoverage > bestCoverage)
            {
                progress = true;
            }
        }
        if (!progress)
        {
            delete[] mutableDimensions;
            return totalCoverage;
        }
    }
}

/// @brief			Returns the mutable dimensions based on a tuple which can be scrambled
/// @param  p_tuple The tuple
/// @param  p_count The returned amount of mutable dimensions
/// @return			The mutable dimensions
template <int DimensionsCount>
int* PairWiseTestGenerator<DimensionsCount>::GetMutableDimensions(FeatureTuple& p_tuple, int& p_count)
{
    std::vector<int>* result = new std::vector<int>();
    bool immutableDimensions[DimensionsCount];

    for (int i = 0; i < DimensionsCount; i++)
    {
        immutableDimensions[i] = false;
    }

    for (int i = 0; i < p_tuple.Count(); i++)
    {
        immutableDimensions[p_tuple[i].Dimension] = true;
    }

    for (int d = 0; d < DimensionsCount; d++)
    {
        if (!immutableDimensions[d])
        {
            result->push_back(d);
        }
    }

    p_count = result->size();
    return result->data();
}

/// @brief				     Randomly scrambles mutable dimensions
/// @param  p_dimensions	 The mutable dimensions
/// @param  p_dimensionCount The amount of mutable dimensions
template <int DimensionsCount>
void PairWiseTestGenerator<DimensionsCount>::ScrambleDimensions(int* p_dimensions, int p_dimensionCount)
{
    for (int i = 0; i < p_dimensionCount; i++)
    {
        int j = m_random.NextInt(p_dimensionCount);
        int t = p_dimensions[i];
        p_dimensions[i] = p_dimensions[j];
        p_dimensions[j] = t;
    }
}

/// @brief				   Maximizes the coverage of a testcase for a dimension
/// @param  p_testCase     The test case
/// @param  p_dimension    The dimension
/// @param  p_bestCoverage The current best coverage
/// @return				   The best possible coverage for this dimension
template <int DimensionsCount>
int PairWiseTestGenerator<DimensionsCount>::MaximizeCoverageForDimension(TestCaseInfo& p_testCase, int p_dimension, int p_bestCoverage)
{
    std::vector<int> bestFeatures(m_dimensions[p_dimension]);

    for (int f = 0; f < m_dimensions[p_dimension]; f++)
    {
        p_testCase.Features[p_dimension] = f;

        int coverage = CountTuplesCoveredByTest(p_testCase, p_dimension, f);

        if (coverage >= p_bestCoverage)
        {
            if (coverage > p_bestCoverage)
            {
                p_bestCoverage = coverage;
                bestFeatures.clear();
            }

            bestFeatures.push_back(f);
        }
    }

    p_testCase.Features[p_dimension] = bestFeatures[m_random.NextInt(bestFeatures.size())];
    return p_bestCoverage;
}

/// @brief				Counts the amount of tuples covered by a test case
/// @param  p_testCase  The test case
/// @param  p_dimension The dimension
/// @param  p_feature   The feature
/// @return             The amount of covered tuples
template <int DimensionsCount>
int PairWiseTestGenerator<DimensionsCount>::CountTuplesCoveredByTest(TestCaseInfo& p_testCase, int p_dimension, int p_feature)
{
    int result = 0;

    for (int i = 0; i < m_uncoveredTuples[p_dimension][p_feature].size(); i++)
    {
        if (p_testCase.IsTupleCovered(m_uncoveredTuples[p_dimension][p_feature].At(i)))
        {
            result++;
        }
    }
    return result;
}

/// @brief			   Removes covered tuples from the uncoveredTuples array
/// @param  p_testCase The new test case
template <int DimensionsCount>
void PairWiseTestGenerator<DimensionsCount>::RemoveTuplesCoveredByTest(TestCaseInfo& p_testCase)
{
    for (int d = 0; d < DimensionsCount; d++)
    {
        for (int f = 0; f < m_dimensions[d]; f++)
        {
            for (int i = m_uncoveredTuples[d][f].size() - 1; i >= 0; i--)
            {
                if (p_testCase.IsTupleCovered(m_uncoveredTuples[d][f].At(i)))
                {
                    m_uncoveredTuples[d][f].RemoveAt(i);
                }
            }
        }
    }
}