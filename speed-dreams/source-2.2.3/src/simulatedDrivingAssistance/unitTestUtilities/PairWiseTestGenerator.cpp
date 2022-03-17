#include "PairWiseTestGenerator.h"
#include "PairWiseTestGenerator.inl"

/// @brief				Constructs a FeatureInfo with a dimension and a feature
/// @param  p_dimension The dimension
/// @param  p_feature   The feature
FeatureInfo::FeatureInfo(int p_dimension, int p_feature)
{
	Dimension = p_dimension;
	Feature = p_feature;
}

/// @brief Default constructor of a FeatureInfo
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

CREATE_PAIR_WISE_TEST_GENERATOR_IMPLEMENTATION(2);
CREATE_PAIR_WISE_TEST_GENERATOR_IMPLEMENTATION(3);
CREATE_PAIR_WISE_TEST_GENERATOR_IMPLEMENTATION(4);
CREATE_PAIR_WISE_TEST_GENERATOR_IMPLEMENTATION(5);
CREATE_PAIR_WISE_TEST_GENERATOR_IMPLEMENTATION(6);
CREATE_PAIR_WISE_TEST_GENERATOR_IMPLEMENTATION(7);
CREATE_PAIR_WISE_TEST_GENERATOR_IMPLEMENTATION(8);
