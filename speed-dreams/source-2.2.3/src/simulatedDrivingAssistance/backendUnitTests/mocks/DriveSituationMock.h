#pragma once
#include "PlayerInfoMock.h"
#include "CarInfoMock.h"
#include "EnvironmentInfoMock.h"

#define CREATE_DRIVE_SITUATION_MOCK \
	PlayerInfoMock playermock(0,0,0,0); \
    TrackPositionMock trackpositionmock(false, 0, 0, 0, 0); \
	CarInfoMock carinfomock(0, 0, 0, 0, trackpositionmock); \
    EnvironmentInfoMock environmentmock(0, 0, 0); \
	DriveSituationMock mock(playermock,carinfomock,environmentmock);

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