#pragma once
#include "car.h"
#include "raceman.h"
#include "boost/interprocess/shared_memory_object.hpp"

static boost::interprocess::shared_memory_object s_segmentDataObject(boost::interprocess::open_or_create, "SDA_SHARED_SEGMENT_MEMORY", boost::interprocess::read_write);

struct BlackBoxData
{
public:
    BlackBoxData(tCarElt* p_car, tSituation* p_situation, int p_tickCount, tTrackSeg* p_nextSegments, int p_nextSegmentsCount);
    ~BlackBoxData();
    tCarElt Car;
    tSituation Situation;
    int TickCount;
};