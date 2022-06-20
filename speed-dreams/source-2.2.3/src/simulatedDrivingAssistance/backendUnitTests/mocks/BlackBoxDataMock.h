/*
 * This program has been developed by students from the bachelor Computer Science at
 * Utrecht University within the Software Project course.
 * © Copyright Utrecht University (Department of Information and Computing Sciences)
 */

#pragma once
#include "car.h"
#include "raceman.h"
#include "Random.hpp"

struct BlackBoxDataMock
{
public:
    BlackBoxDataMock(tCarElt* p_car, tSituation* p_situation, unsigned long p_tickCount, tTrackSeg* p_trackSegments, int p_trackSegmentsCount)
        : Car(*p_car),
        Situation(*p_situation),
        TickCount(p_tickCount) {}
    tCarElt Car;
    tSituation Situation;
    unsigned long TickCount;
};

inline BlackBoxDataMock GetExampleBlackBoxDataMock()
{
    tCarElt* car = new tCarElt();
    tSituation* situation = new tSituation();
    unsigned long tickCount = 0;

    return { car, situation, tickCount, nullptr, 0 };
}

inline BlackBoxDataMock CreateRandomBlackBoxDataMock(Random& p_random)
{
    tCarElt* car = new tCarElt();
    tSituation* situation = new tSituation();
    unsigned long tickCount = p_random.NextInt(0, 1000);

    return { car, situation, tickCount, nullptr, 0 };
}