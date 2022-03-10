#pragma once
#include "TrackPosition.h"
class EnvironmentInfo
{
private:
    TrackPosition m_trackPosition;
public:
	bool Offroad(){return false;};
	int TimeOfDay(){return 0;};
	int Clouds(){return 0;};

	TrackPosition* TrackLocalPosition()
    {
        return &m_trackPosition;
    }
};