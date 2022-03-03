#pragma once
#include "TrackPosition.h"
class EnvironmentInfo
{
public:
	bool Offroad(){return false;};
	int TimeOfDay(){return 0;};
	int Clouds(){return 0;};
	TrackPosition TrackLocalPosition()
    {
        TrackPosition trackPosition;
        return trackPosition;
    }
};