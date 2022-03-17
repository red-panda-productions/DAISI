#pragma once
#include "../rppUtils/Random.hpp"
#include <queue>
// based on https://github.com/nunit/nunit/blob/b34eba3ac1aa6957157857bddd116256c634afab/src/NUnitFramework/framework/Internal/Builders/PairwiseStrategy.cs

class FeatureInfo
{
public:
	int Dimension;
	int Feature;
	FeatureInfo(int p_dimension, int p_feature);
	FeatureInfo();
};

class FeatureTuple
{
public:
	FeatureTuple();
	FeatureTuple(FeatureInfo& p_feature);
	FeatureTuple(FeatureInfo& p_feature1, FeatureInfo& p_feature2);

	int Count();
	FeatureInfo& operator[](int p_index);
	void SetTuple(FeatureInfo& p_feature);
	void SetTuple(FeatureInfo& p_feature1, FeatureInfo& p_feature2);
private:
	FeatureInfo m_features[2];
	int m_count;
};

class TestCaseInfo
{
public:
	int* Features;
	int Length;

	TestCaseInfo(int p_length);

	TestCaseInfo();

	void SetTestCaseInfo(int p_length);

	bool IsTupleCovered(FeatureTuple& p_tuple);
};

template<int m_dimensionsCount>
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

	Random m_random;
	int m_dimensions[m_dimensionsCount] = {0};
	Queue* m_uncoveredTuples[m_dimensionsCount] = {nullptr};


	void CreateAllTuples();

	void CreateTuples(int p_dimension, int p_feature, Queue& p_uncoveredTuple);

	bool GetNextTuple(FeatureTuple& tuple);

	void CreateTestCase(FeatureTuple& p_tuple, TestCaseInfo& p_testCase);

	void CreateRandomTestCase(FeatureTuple& p_tuple, TestCaseInfo& p_testCase);

	int MaximizeCoverage(TestCaseInfo& p_testCase, FeatureTuple& p_tuple);

	int* GetMutableDimensions(FeatureTuple& p_tuple, int& p_count);

	void ScrambleDimensions(int* p_dimensions, int p_dimensionCount);

	int MaximizeCoverageForDimension(TestCaseInfo& p_testCase, int p_dimension, int p_bestCoverage);

	int CountTuplesCoveredByTest(TestCaseInfo& p_testCase, int p_dimension, int p_feature);

	void RemoveTuplesCoveredByTest(TestCaseInfo& p_testCase);


};