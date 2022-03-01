#pragma once
#include "EnvironmentInfo.h"
#include "CarInfo.h"
#include "PlayerInfo.h"


class DriveSituation
{
public:
	EnvironmentInfo GetEnvironmentInfo();
	CarInfo GetCarInfo();
	PlayerInfo GetPlayerInfo();
};