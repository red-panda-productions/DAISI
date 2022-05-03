#pragma once
#include "car.h"
#include "raceman.h"
#include <gtest/gtest-spi.h>

/// @brief Inverts a test. ONLY INTENDED FOR calling with functions provided in this header file, and only with p_eqOrNe equal to true. \n
/// Since these comparisons have many tests, a couple naive methods of inverting don't work: \n
/// introducing a variable that switchers every EQ with NE and vice versa; would require inequality of all values, \n
/// EXPECT_(NON)FATAL_FAILURE; expects a single failure, \n
/// chaining EXPECT_NONFATAL_FAILURE with the above; fails if there is just one failure
/// @note Scoped so the reporter is not useable outside of this inversion
/// @note Strictly speaking will not invert a test with p_eqOrNe fully, as having a single pointer equal would already fail the inner test
/// (and thus succeed), but calling INVERT_COMP_TEST with an inner call with p_eqOrNe == false is not the intended use case
#define INVERT_COMP_TEST(a)                                                                                                                        \
    {                                                                                                                                              \
        testing::TestPartResultArray failures;                                                                                                     \
        testing::ScopedFakeTestPartResultReporter reporter(::testing::ScopedFakeTestPartResultReporter::INTERCEPT_ONLY_CURRENT_THREAD, &failures); \
        (a);                                                                                                                                       \
        EXPECT_GE(failures.size(), 1);                                                                                                             \
    }

void CompareCars(tCarElt& p_car1, tCarElt& p_car2, bool p_eqOrNe);

void CompareSituations(tSituation& p_situation1, tSituation& p_situation2, bool p_eqOrNe);

void CompareSegments(tTrackSeg* p_segments1, tTrackSeg* p_segments2, int p_segmentsCount, bool p_eqOrNe);