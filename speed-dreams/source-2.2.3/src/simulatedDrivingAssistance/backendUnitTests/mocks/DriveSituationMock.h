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

    PlayerInfoMock* GetPlayerInfo(){return &m_playerInfo;}
    EnvironmentInfoMock* GetEnvironmentInfo(){return &m_environmentInfo;};
    CarInfoMock* GetCarInfo(){return &m_carInfo;};
};

/// @brief  A sample drive situation for testing
DriveSituationMock GetExampleDriveSituation()
{
    TrackPositionMock trackpos(true, 0.1, 0.2, 0.3, 0.4);
    CarInfoMock carinfo(1.1, 1.2, 1, true, trackpos);
    PlayerInfoMock playerinfo(2.1, 2.2, 2.3, 2.4);
    EnvironmentInfoMock environmentinfo(3, 4, 5);
    return { playerinfo, carinfo, environmentinfo };
}