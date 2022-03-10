#pragma once
#include "PlayerInfoMock.h"
#include "CarInfoMock.h"
#include "EnvironmentInfoMock.h"

class DriveSituationMock
{
public:
    DriveSituationMock(PlayerInfoMock m_PlayerInfo, CarInfoMock mCarInfo, EnvironmentInfoMock mEnvironmentInfo)
            : m_playerInfo(m_PlayerInfo), m_carInfo(mCarInfo), m_environmentInfo(mEnvironmentInfo) {};

    PlayerInfoMock m_playerInfo;
    EnvironmentInfoMock m_environmentInfo;
    CarInfoMock m_carInfo;

    PlayerInfoMock GetPlayerInfo(){return m_playerInfo;}
    EnvironmentInfoMock GetEnvironmentInfo(){return m_environmentInfo;};
    CarInfoMock GetCarInfo(){return m_carInfo;};
};