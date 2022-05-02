#pragma once
#include "car.h"
#include "raceman.h"

struct TestSegments
{
    tTrackSeg* NextSegments;
    int NextSegmentsCount;
};

class Generator
{
public:
    static tTrackSeg GenerateSegment();

    static TestSegments GenerateSegments();

    static void DestroySegments(TestSegments& p_segments);

    static tCarElt GenerateCar(TestSegments& p_segments);

    static void DestroyCar(tCarElt& p_car);

    static tSituation GenerateSituation();

    static void DestroySituation(tSituation& p_situation);
};