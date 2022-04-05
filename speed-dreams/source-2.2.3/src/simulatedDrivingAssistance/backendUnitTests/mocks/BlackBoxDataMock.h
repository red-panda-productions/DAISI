#pragma once
#include "car.h"
#include "raceman.h"
#include "../rppUtils/Random.hpp"

struct BlackBoxDataMock
{
public:
    BlackBoxDataMock(tCarElt* p_car, tSituation* p_situation, int p_tickCount, tTrackSeg* p_trackSegments, int p_trackSegmentsCount)
    {
        Car = *p_car;
        Situation = *p_situation;
        TickCount = p_tickCount;
    }
    tCarElt Car;
    tSituation Situation;
    int TickCount;
};

inline BlackBoxDataMock GetExampleBlackBoxDataMock()
{
    tCarElt* car = new tCarElt();
    tSituation* situation = new tSituation();
    int tickCount = 0;

    return { car, situation, tickCount, nullptr, 0 };
}

inline BlackBoxDataMock CreateRandomBlackBoxDataMock(Random& random)
{
    tCarElt* car = new tCarElt();
    tSituation* situation = new tSituation();
    int tickCount = random.NextInt(0,1000);

    return { car, situation, tickCount, nullptr, 0 };
}