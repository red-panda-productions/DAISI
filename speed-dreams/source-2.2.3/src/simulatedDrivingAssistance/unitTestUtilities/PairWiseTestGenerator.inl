#pragma once
#include "PairWiseTestGenerator.h"

#define CREATE_PAIR_WISE_TEST_GENERATOR_IMPLEMENTATION(dimensions) \
	template std::vector<TestCaseInfo>* PairWiseTestGenerator<dimensions>::GetTestCases(int* p_dimensions);\
	template void PairWiseTestGenerator<dimensions>::CreateAllTuples(); \
	template void PairWiseTestGenerator<dimensions>::CreateTuples(int p_dimension, int p_feature, Queue& p_uncoveredTuple); \
	template bool PairWiseTestGenerator<dimensions>::GetNextTuple(FeatureTuple& tuple); \
	template void PairWiseTestGenerator<dimensions>::CreateTestCase(FeatureTuple& p_tuple, TestCaseInfo& p_testCase); \
	template void PairWiseTestGenerator<dimensions>::CreateRandomTestCase(FeatureTuple& p_tuple, TestCaseInfo& p_testCase); \
	template int PairWiseTestGenerator<dimensions>::MaximizeCoverage(TestCaseInfo& p_testCase, FeatureTuple& p_tuple); \
	template int* PairWiseTestGenerator<dimensions>::GetMutableDimensions(FeatureTuple& p_tuple, int& p_count); \
	template void PairWiseTestGenerator<dimensions>::ScrambleDimensions(int* p_dimensions, int p_dimensionCount); \
	template int PairWiseTestGenerator<dimensions>::MaximizeCoverageForDimension(TestCaseInfo& p_testCase, int p_dimension, int p_bestCoverage); \
	template int PairWiseTestGenerator<dimensions>::CountTuplesCoveredByTest(TestCaseInfo& p_testCase, int p_dimension, int p_feature); \
	template void PairWiseTestGenerator<dimensions>::RemoveTuplesCoveredByTest(TestCaseInfo& p_testCase)

template<int m_dimensionsCount>
std::vector<TestCaseInfo>* PairWiseTestGenerator<m_dimensionsCount>::GetTestCases(int* p_dimensions)
{
	for (int i = 0; i < m_dimensionsCount; i++)
	{
		m_dimensions[i] = p_dimensions[i];
	}

	CreateAllTuples();

	std::vector<TestCaseInfo>* testCases = new std::vector<TestCaseInfo>();

	while (true)
	{
		FeatureTuple tuple;

		if (!GetNextTuple(tuple)) break;

		TestCaseInfo testCase(m_dimensionsCount);

		CreateTestCase(tuple, testCase);

		RemoveTuplesCoveredByTest(testCase);

		testCases->push_back(testCase);
	}

	for (int i = 0; i < m_dimensionsCount; i++)
	{
		delete[] m_uncoveredTuples[i];
	}

	return testCases;
}

template<int m_dimensionsCount>
void PairWiseTestGenerator<m_dimensionsCount>::CreateAllTuples()
{
	for (int d = 0; d < m_dimensionsCount; d++)
	{
		m_uncoveredTuples[d] = new Queue[m_dimensions[d]];
		for (int f = 0; f < m_dimensions[d]; f++)
		{
			CreateTuples(d, f, m_uncoveredTuples[d][f]);
		}
	}
}

template<int m_dimensionsCount>
void PairWiseTestGenerator<m_dimensionsCount>::CreateTuples(int p_dimension, int p_feature, Queue& p_uncoveredTuple)
{
	FeatureInfo f1 = FeatureInfo(p_dimension, p_feature);
	FeatureTuple t = FeatureTuple(f1);
	p_uncoveredTuple.emplace(t);
	FeatureInfo f2;
	for (int d = 0; d < m_dimensionsCount; d++)
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

template<int m_dimensionsCount>
bool PairWiseTestGenerator<m_dimensionsCount>::GetNextTuple(FeatureTuple& tuple)
{
	for (int d = 0; d < m_dimensionsCount; d++)
	{
		for (int f = 0; f < m_dimensions[d]; f++)
		{
			if (m_uncoveredTuples[d][f].empty()) continue;

			tuple = m_uncoveredTuples[d][f].front();
			m_uncoveredTuples[d][f].pop();
			return true;
		}
	}
	return false;
}

template<int m_dimensionsCount>
void PairWiseTestGenerator<m_dimensionsCount>::CreateTestCase(FeatureTuple& p_tuple, TestCaseInfo& p_testCase)
{
	int bestCoverage = -1;

	for (int i = 0; i < 7; i++)
	{
		TestCaseInfo testCase(m_dimensionsCount);
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

template<int m_dimensionsCount>
void PairWiseTestGenerator<m_dimensionsCount>::CreateRandomTestCase(FeatureTuple& p_tuple, TestCaseInfo& p_testCase)
{
	for (int d = 0; d < m_dimensionsCount; d++)
	{
		p_testCase.Features[d] = m_random.NextInt(m_dimensions[d]);
	}

	for (int i = 0; i < p_tuple.Count(); i++)
	{
		p_testCase.Features[p_tuple[i].Dimension] = p_tuple[i].Feature;
	}
}

template<int m_dimensionsCount>
int PairWiseTestGenerator<m_dimensionsCount>::MaximizeCoverage(TestCaseInfo& p_testCase, FeatureTuple& p_tuple)
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
			return totalCoverage;
		}
	}
}

template<int m_dimensionsCount>
int* PairWiseTestGenerator<m_dimensionsCount>::GetMutableDimensions(FeatureTuple& p_tuple, int& p_count)
{
	std::vector<int>* result = new std::vector<int>();
	bool immutableDimensions[m_dimensionsCount];

	for (int i = 0; i < m_dimensionsCount; i++)
	{
		immutableDimensions[i] = false;
	}

	for (int i = 0; i < p_tuple.Count(); i++)
	{
		immutableDimensions[p_tuple[i].Dimension] = true;
	}

	for (int d = 0; d < m_dimensionsCount; d++)
	{
		if (!immutableDimensions[d])
		{
			result->push_back(d);
		}
	}

	p_count = result->size();
	return result->data();
}

template<int m_dimensionsCount>
void PairWiseTestGenerator<m_dimensionsCount>::ScrambleDimensions(int* p_dimensions, int p_dimensionCount)
{
	for (int i = 0; i < p_dimensionCount; i++)
	{
		int j = m_random.NextInt(p_dimensionCount);
		int t = p_dimensions[i];
		p_dimensions[i] = p_dimensions[j];
		p_dimensions[j] = t;
	}
}

template<int m_dimensionsCount>
int PairWiseTestGenerator<m_dimensionsCount>::MaximizeCoverageForDimension(TestCaseInfo& p_testCase, int p_dimension, int p_bestCoverage)
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

template<int m_dimensionsCount>
int PairWiseTestGenerator<m_dimensionsCount>::CountTuplesCoveredByTest(TestCaseInfo& p_testCase, int p_dimension, int p_feature)
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

template<int m_dimensionsCount>
void PairWiseTestGenerator<m_dimensionsCount>::RemoveTuplesCoveredByTest(TestCaseInfo& p_testCase)
{
	for (int d = 0; d < m_dimensionsCount; d++)
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