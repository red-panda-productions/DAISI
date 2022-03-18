#pragma once
#include "EnvironmentInfo.h"
#include "CarInfo.h"
#include "PlayerInfo.h"

class DriveSituation
{
public:
    DriveSituation();
    DriveSituation(EnvironmentInfo& p_environmentInfo, CarInfo& p_carInfo, PlayerInfo& p_playerInfo);
    EnvironmentInfo* GetEnvironmentInfo();
    CarInfo* GetCarInfo();
    PlayerInfo* GetPlayerInfo();

private:
    EnvironmentInfo m_environmentInfo;
    CarInfo m_carInfo;
    PlayerInfo m_playerInfo;
};