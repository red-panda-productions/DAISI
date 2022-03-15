#pragma once
#include "EnvironmentInfo.h"
#include "CarInfo.h"
#include "PlayerInfo.h"


class DriveSituation
{
private:
    EnvironmentInfo m_environmentInfo;
    CarInfo m_carInfo;
    PlayerInfo m_playerInfo;

public:
    EnvironmentInfo* GetEnvironmentInfo();
    CarInfo* GetCarInfo();
    PlayerInfo* GetPlayerInfo();
};