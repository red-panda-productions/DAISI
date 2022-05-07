#pragma once
#include "car.h"
#include "raceman.h"

struct TestSegments
{
    tTrackSeg* NextSegments;
    int NextSegmentsCount;
};

tTrackSeg GenerateSegment();

TestSegments GenerateSegments();

void DestroySegments(TestSegments& p_segments);

tCarElt GenerateCar(TestSegments& p_segments);

void DestroyCar(tCarElt& p_car);

tSituation GenerateSituation();

void DestroySituation(tSituation& p_situation);