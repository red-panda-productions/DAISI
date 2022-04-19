#pragma once
#include "car.h"
#include "raceman.h"

struct BlackBoxData
{
public:
    BlackBoxData(tCarElt* p_car, tSituation* p_situation, unsigned long p_tickCount, tTrackSeg* p_nextSegments, int p_nextSegmentsCount);
    tCarElt Car;
    tSituation Situation;
    unsigned long TickCount;
};