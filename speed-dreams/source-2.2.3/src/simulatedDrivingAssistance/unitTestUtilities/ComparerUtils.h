#pragma once
#include "car.h"
#include "raceman.h"

class ComparerUtils
{
public:
    static void CompareCars(tCarElt& p_car1, tCarElt& p_car2, bool p_eqOrNe, bool p_negate);

    static void CompareSituations(tSituation& p_situation1, tSituation& p_situation2, bool p_eqOrNe, bool p_negate);

    static void CompareSegments(tTrackSeg* p_segments1, tTrackSeg* p_segments2, int p_segmentsCount, bool p_eqOrNe, bool p_negate);
};