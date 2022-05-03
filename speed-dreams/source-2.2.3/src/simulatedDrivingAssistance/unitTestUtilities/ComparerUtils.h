#pragma once
#include "car.h"
#include "raceman.h"
#include <gtest/gtest-spi.h>

/// @brief Inverts a test. Since these comparisons have many tests, a couple naive methods of inverting don't work: \
/// introducing a variable that switchers every EQ with NE and vice versa; would require inequality of all values, \
/// EXPECT_(NON)FATAL_FAILURE; expects a single failure, \
/// chaining EXPECT_NONFATAL_FAILURE with the above; fails if there is just one failure
/// @note Scoped so the reporter is not useable outside of this inversion
#define INVERT_COMP_TEST(a)                                                                                                                        \
    {                                                                                                                                              \
        testing::TestPartResultArray failures;                                                                                                     \
        testing::ScopedFakeTestPartResultReporter reporter(::testing::ScopedFakeTestPartResultReporter::INTERCEPT_ONLY_CURRENT_THREAD, &failures); \
        (a);                                                                                                                                       \
        EXPECT_GE(failures.size(), 1);                                                                                                             \
    }

class ComparerUtils
{
public:
    static void CompareCars(tCarElt& p_car1, tCarElt& p_car2, bool p_eqOrNe);

    static void CompareSituations(tSituation& p_situation1, tSituation& p_situation2, bool p_eqOrNe);

    static void CompareSegments(tTrackSeg* p_segments1, tTrackSeg* p_segments2, int p_segmentsCount, bool p_eqOrNe);
};