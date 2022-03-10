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
	EnvironmentInfo* GetEnvironmentInfo()
    {
        return &m_environmentInfo;
    };
	CarInfo* GetCarInfo()
    {
        return &m_carInfo;
    };
	PlayerInfo* GetPlayerInfo()
    {
        return &m_playerInfo;
    };
};