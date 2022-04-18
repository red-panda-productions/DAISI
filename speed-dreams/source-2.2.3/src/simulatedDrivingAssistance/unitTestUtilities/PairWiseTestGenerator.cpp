#include "PairWiseTestGenerator.h"
#include "PairWiseTestGenerator.inl"

/// @brief				Constructs a FeatureInfo with a dimension and a feature
/// @param  p_dimension The dimension
/// @param  p_feature   The feature
FeatureInfo::FeatureInfo(int p_dimension, int p_feature)
    : Dimension(p_dimension),
      Feature(p_feature) {}

/// @brief Default constructor of a FeatureInfo
FeatureInfo::FeatureInfo()
    : Dimension(-1),
      Feature(-1) {}

/// @brief			  Constructs a FeatureTuple with 1 feature
/// @param  p_feature The feature
FeatureTuple::FeatureTuple(FeatureInfo& p_feature)
{
    SetTuple(p_feature);
}

/// @brief			   Constructs a FeatureTuple with 2 features
/// @param  p_feature1 The first feature
/// @param  p_feature2 The second feature
FeatureTuple::FeatureTuple(FeatureInfo& p_feature1, FeatureInfo& p_feature2)
{
    SetTuple(p_feature1, p_feature2);
}

/// @brief  Gives back the internal count of this tuple
/// @return The amount of features in this tuple
int FeatureTuple::Count() const
{
    return m_count;
}

/// @brief			An accessor to access the internal features
/// @param  p_index The index to access
/// @return			The feature info at the given index
FeatureInfo& FeatureTuple::operator[](int p_index)
{
    return m_features[p_index];
}

/// @brief			  Sets the internal data of the tuple
/// @param  p_feature The feature to set
void FeatureTuple::SetTuple(const FeatureInfo& p_feature)
{
    m_features[0].Dimension = p_feature.Dimension;
    m_features[0].Feature = p_feature.Feature;
    m_count = 1;
}

/// @brief			   Sets the internal data of the tuple
/// @param  p_feature1 The first feature to set
/// @param  p_feature2 The second feature to set
void FeatureTuple::SetTuple(FeatureInfo& p_feature1, FeatureInfo& p_feature2)
{
    m_features[0].Dimension = p_feature1.Dimension;
    m_features[0].Feature = p_feature1.Feature;
    m_features[1].Dimension = p_feature2.Dimension;
    m_features[1].Feature = p_feature2.Feature;
    m_count = 2;
}

/// @brief			 Constructs a TestCaseInfo with a given length
/// @param  p_length The length
TestCaseInfo::TestCaseInfo(int p_length)
    : Features(new int[p_length]),
      Length(p_length) {}

/// @brief The default constructor of TestCaseInfo
TestCaseInfo::TestCaseInfo()
    : Features(nullptr),
      Length(0) {}

/// @brief			 Set the internal data of the test info
/// @param  p_length The length
void TestCaseInfo::SetTestCaseInfo(int p_length)
{
    delete[] Features;
    Features = new int[p_length];
    Length = p_length;
}

/// @brief			Checks if a testCase is covered by a tuple
/// @param  p_tuple The tuple
/// @return			Whether the test case is covered
bool TestCaseInfo::IsTupleCovered(FeatureTuple& p_tuple) const
{
    for (int i = 0; i < p_tuple.Count(); i++)
    {
        if (Features[p_tuple[i].Dimension] != p_tuple[i].Feature) return false;
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
