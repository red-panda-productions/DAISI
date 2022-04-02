#pragma once
#include "car.h"
#include "raceman.h"

struct BlackBoxData
{
public:
    BlackBoxData(tCarElt* p_car, tSituation* p_situation, int p_tickCount);
    ~BlackBoxData();
    tCarElt Car;
    tSituation Situation;
    int TickCount;
};