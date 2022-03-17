#include "PairWiseTestGenerator.h"

FeatureInfo::FeatureInfo(int p_dimension, int p_feature)
{
	Dimension = p_dimension;
	Feature = p_feature;
}
FeatureInfo::FeatureInfo()
{
	Dimension = -1;
	Feature = -1;
}

FeatureTuple::FeatureTuple()
{
	m_count = 0;
}


FeatureTuple::FeatureTuple(FeatureInfo& p_feature)
{
	SetTuple(p_feature);
}


FeatureTuple::FeatureTuple(FeatureInfo& p_feature1, FeatureInfo& p_feature2)
{
	SetTuple(p_feature1, p_feature2);
}


int FeatureTuple::Count()
{
	return m_count;
}

FeatureInfo& FeatureTuple::operator[](int p_index)
{
	return m_features[p_index];
}

void FeatureTuple::SetTuple(FeatureInfo& p_feature)
{
	m_features[0].Dimension = p_feature.Dimension;
	m_features[0].Feature = p_feature.Feature;
	m_count = 1;
}

void FeatureTuple::SetTuple(FeatureInfo& p_feature1, FeatureInfo& p_feature2)
{
	m_features[0].Dimension = p_feature1.Dimension;
	m_features[0].Feature = p_feature1.Feature;
	m_features[1].Dimension = p_feature2.Dimension;
	m_features[1].Feature = p_feature2.Feature;
	m_count = 2;
}

TestCaseInfo::TestCaseInfo(int p_length)
{
	Features = new int[p_length];
	Length = p_length;
}

TestCaseInfo::TestCaseInfo()
{
	Features = nullptr;
	Length = 0;
}

void TestCaseInfo::SetTestCaseInfo(int p_length)
{
	delete[] Features;
	Features = new int[p_length];
	Length = p_length;
}

bool TestCaseInfo::IsTupleCovered(FeatureTuple& p_tuple)
{
	for(int i = 0; i < p_tuple.Count(); i++)
	{
		if(Features[p_tuple[i].Dimension] != p_tuple[i].Feature) return false;
	}
	return true;
}


std::vector<TestCaseInfo>* PairWiseTestGenerator::GetTestCases(int* p_dimensions, int p_dimensionsCount)
{
	m_dimensions = p_dimensions;
	m_dimensionsCount = p_dimensionsCount;

	CreateAllTuples();

	std::vector<TestCaseInfo>* testCases = new std::vector<TestCaseInfo>();

	while (true)
	{
		FeatureTuple tuple;

		if(!GetNextTuple(tuple)) break;

		TestCaseInfo testCase(p_dimensionsCount);

		CreateTestCase(tuple, testCase);

		RemoveTuplesCoveredByTest(testCase);

		testCases->push_back(testCase);
	}

	for(int i = 0; i < m_dimensionsCount; i++)
	{
		delete[] m_uncoveredTuples[i];
	}
	delete[] m_uncoveredTuples;

	return testCases;
}

void PairWiseTestGenerator::CreateAllTuples()
{
	m_uncoveredTuples = new Queue*[m_dimensionsCount];
	for(int d = 0; d < m_dimensionsCount; d++)
	{
		m_uncoveredTuples[d] = new Queue[m_dimensions[d]];
		for(int f = 0; f < m_dimensions[d]; f++)
		{
			CreateTuples(d, f, m_uncoveredTuples[d][f]);
		}
	}
}

void PairWiseTestGenerator::CreateTuples(int p_dimension, int p_feature, Queue& p_uncoveredTuple)
{
	FeatureInfo f1 = FeatureInfo(p_dimension, p_feature);
	FeatureTuple t = FeatureTuple(f1);
	p_uncoveredTuple.emplace(t);
	FeatureInfo f2;
	for(int d = 0; d < m_dimensionsCount; d++)
	{
		if (d == p_dimension) continue;
		for(int f = 0; f < m_dimensions[d]; f++)
		{
			f2.Dimension = d;
			f2.Feature = f;
			t.SetTuple(f1, f2);
			p_uncoveredTuple.emplace(t);
		}
	}
}


bool PairWiseTestGenerator::GetNextTuple(FeatureTuple& tuple)
{
	for(int d = 0; d < m_dimensionsCount; d++)
	{
		for(int f = 0; f < m_dimensions[d]; f++)
		{
			if (m_uncoveredTuples[d][f].empty()) continue;

			tuple = m_uncoveredTuples[d][f].front();
			m_uncoveredTuples[d][f].pop();
			return true;
		}
	}
	return false;
}

void PairWiseTestGenerator::CreateTestCase(FeatureTuple& p_tuple, TestCaseInfo& p_testCase)
{
	int bestCoverage = -1;

	for(int i = 0; i < 7; i++)
	{
		TestCaseInfo testCase(m_dimensionsCount);
		CreateRandomTestCase(p_tuple, testCase);
		int coverage = MaximizeCoverage(testCase, p_tuple);
		if(coverage > bestCoverage)
		{
			for(int i = 0; i < testCase.Length; i++)
			{
				p_testCase.Features[i] = testCase.Features[i];
			}
			bestCoverage = coverage;
		}
	}
}

void PairWiseTestGenerator::CreateRandomTestCase(FeatureTuple& p_tuple, TestCaseInfo& p_testCase)
{
	for(int d = 0; d < m_dimensionsCount; d++)
	{
		p_testCase.Features[d] = m_random.NextInt(m_dimensions[d]);
	}

	for(int i = 0; i < p_tuple.Count(); i++)
	{
		p_testCase.Features[p_tuple[i].Dimension] = p_tuple[i].Feature;
	}
}

int PairWiseTestGenerator::MaximizeCoverage(TestCaseInfo& p_testCase, FeatureTuple& p_tuple)
{
	int totalCoverage = 1;
	int mutableCount = 0;
	int* mutableDimensions = GetMutableDimensions(p_tuple,mutableCount);

	while(true)
	{
		bool progress = false;

		ScrambleDimensions(mutableDimensions,mutableCount);

		for(int i = 0; i < mutableCount; i++)
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
		if(!progress)
		{
			return totalCoverage;
		}
	}
}

int* PairWiseTestGenerator::GetMutableDimensions(FeatureTuple& p_tuple, int& p_count)
{
	std::vector<int>* result = new std::vector<int>();
	bool* immutableDimensions = new bool[m_dimensionsCount];

	for(int i = 0; i < m_dimensionsCount; i++)
	{
		immutableDimensions[i] = false;
	}

	for(int i = 0; i < p_tuple.Count(); i++)
	{
		immutableDimensions[p_tuple[i].Dimension] = true;
	}

	for(int d = 0; d < m_dimensionsCount; d++)
	{
		if (!immutableDimensions[d])
		{
			result->push_back(d);
		}
	}


	delete[] immutableDimensions;
	p_count = result->size();
	return result->data();
}

void PairWiseTestGenerator::ScrambleDimensions(int* p_dimensions, int p_dimensionCount)
{
	for(int i = 0; i < p_dimensionCount; i++)
	{
		int j = m_random.NextInt(p_dimensionCount);
		int t = p_dimensions[i];
		p_dimensions[i] = p_dimensions[j];
		p_dimensions[j] = t;
	}
}

int PairWiseTestGenerator::MaximizeCoverageForDimension(TestCaseInfo& p_testCase, int p_dimension, int p_bestCoverage)
{
	std::vector<int> bestFeatures(m_dimensions[p_dimension]);

	for(int f = 0; f < m_dimensions[p_dimension]; f++)
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

int PairWiseTestGenerator::CountTuplesCoveredByTest(TestCaseInfo& p_testCase, int p_dimension, int p_feature)
{
	int result = 0;

	for(int i = 0; i < m_uncoveredTuples[p_dimension][p_feature].size(); i++)
	{
		if (p_testCase.IsTupleCovered(m_uncoveredTuples[p_dimension][p_feature].At(i)))
		{
			result++;
		}
	}
	return result;
}

void PairWiseTestGenerator::RemoveTuplesCoveredByTest(TestCaseInfo& p_testCase)
{
	for(int d = 0; d < m_dimensionsCount; d++)
	{
		for(int f = 0; f < m_dimensions[d]; f++)
		{
			for(int i = m_uncoveredTuples[d][f].size() - 1; i >= 0; i--)
			{
				if(p_testCase.IsTupleCovered(m_uncoveredTuples[d][f].At(i)))
				{
					m_uncoveredTuples[d][f].RemoveAt(i);
				}
			}
		}
	}
}

