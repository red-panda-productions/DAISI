#pragma once
#include "EnvironmentInfo.h"
#include "CarInfo.h"
#include "PlayerInfo.h"


class DriveSituation
{
public:
	EnvironmentInfo GetEnvironmentInfo()
    {
        EnvironmentInfo environmentInfo;
        return environmentInfo;
    };
	CarInfo GetCarInfo()
    {
        CarInfo carInfo;
        return carInfo;
    };
	PlayerInfo GetPlayerInfo()
    {
        PlayerInfo playerInfo;
        return playerInfo;
    };
};