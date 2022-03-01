#pragma once
#include "TrackPosition.h"
class EnvironmentInfo
{
public:
	bool Offroad();
	int TimeOfDay();
	int Clouds();
	TrackPosition TrackLocalPosition();
};