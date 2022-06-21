/*
 * This program has been developed by students from the bachelor Computer Science at
 * Utrecht University within the Software Project course.
 * © Copyright Utrecht University (Department of Information and Computing Sciences)
 */

#pragma once
#include "car.h"
#include "carstruct.h"
#include "raceman.h"

struct BlackBoxData
{
public:
    BlackBoxData(tCar* p_carTable, tCarElt* p_car, tSituation* p_situation, unsigned long p_tickCount, tTrackSeg* p_nextSegments, int p_nextSegmentsCount);
    BlackBoxData() = default;
    ~BlackBoxData();

    tCar SimCar;
    tCarElt Car;
    tSituation Situation;
    unsigned long TickCount;
};