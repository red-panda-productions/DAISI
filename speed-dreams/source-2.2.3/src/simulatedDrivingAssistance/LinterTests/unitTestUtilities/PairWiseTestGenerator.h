#pragma once
#include "../../rppUtils/Random.hpp"
#include <queue>
// based on https://github.com/nunit/nunit/blob/b34eba3ac1aa6957157857bddd116256c634afab/src/NUnitFramework/framework/Internal/Builders/PairwiseStrategy.cs

/// @brief Represents coverage of a single value of test function
///		   parameter, represented as a pair of indices, Dimension and Feature. In
///		   terms of unit testing, Dimension is the index of the test parameter and
///		   Feature is the index of the supplied value in that parameter's list of
///		   sources.
class FeatureInfo
{
public:
	int Dimension;
	int Feature;
	FeatureInfo(int p_dimension, int p_feature);
	FeatureInfo();
};

/// @brief Represents a combination of features, one per test
///		   parameter, which should be covered by a test case. In the
///		   Pairwise strategy, we are only trying to cover pairs of features, so the
///		   tuples actually may contain only single feature or pair of features, but
///		   the algorithms itself works with triplets, quadruples and so on
class FeatureTuple
{
public:
	FeatureTuple() = default;
	explicit FeatureTuple(FeatureInfo& p_feature);
	explicit FeatureTuple(FeatureInfo& p_feature1, FeatureInfo& p_feature2);

	int Count() const;
	FeatureInfo& operator[](int p_index);
	void SetTuple(const FeatureInfo& p_feature);
	void SetTuple(FeatureInfo& p_feature1, FeatureInfo& p_feature2);
private:
	FeatureInfo m_features[2];
	int m_count = 0;
};

/// @brief Represents a single test case covering a list of features
class TestCaseInfo
{
public:
	int* Features;
	int Length;

	explicit TestCaseInfo(int p_length);

	TestCaseInfo();

	void SetTestCaseInfo(int p_length);

	bool IsTupleCovered(FeatureTuple& p_tuple) const;
};

/// @brief					  Implements an algorithm which generates a set of test cases
///							  which covers all pairs of possible values of test function
/// @tparam DimensionsCount The amount of dimensions the generator should use
template<int DimensionsCount>
class PairWiseTestGenerator
{
public:
	std::vector<TestCaseInfo>* GetTestCases(int* p_dimensions);

private:
	class Queue : public std::queue<FeatureTuple>
	{
	public:
		FeatureTuple& At(int p_index)
		{
			return this->c.at(p_index);
		}
		void RemoveAt(int p_index)
		{
			this->c.erase(c.begin()+p_index);
		}
	};

	Random m_random = Random();
	int m_dimensions[DimensionsCount] = {0};
	Queue* m_uncoveredTuples[DimensionsCount] = {nullptr};


	void CreateAllTuples();

	void CreateTuples(int p_dimension, int p_feature, Queue& p_uncoveredTuple);

	bool GetNextTuple(FeatureTuple& p_tuple);

	void CreateTestCase(FeatureTuple& p_tuple, TestCaseInfo& p_testCase);

	void CreateRandomTestCase(FeatureTuple& p_tuple, TestCaseInfo& p_testCase);

	int MaximizeCoverage(TestCaseInfo& p_testCase, FeatureTuple& p_tuple);

	int* GetMutableDimensions(FeatureTuple& p_tuple, int& p_count);

	void ScrambleDimensions(int* p_dimensions, int p_dimensionCount);

	int MaximizeCoverageForDimension(TestCaseInfo& p_testCase, int p_dimension, int p_bestCoverage);

	int CountTuplesCoveredByTest(TestCaseInfo& p_testCase, int p_dimension, int p_feature);

	void RemoveTuplesCoveredByTest(TestCaseInfo& p_testCase);
};