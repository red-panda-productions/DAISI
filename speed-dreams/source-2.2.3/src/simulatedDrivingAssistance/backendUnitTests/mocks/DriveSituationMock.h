#pragma once
#include "PlayerInfoMock.h"
#include "CarInfoMock.h"
#include "EnvironmentInfoMock.h"

#define CREATE_DRIVE_SITUATION_MOCK \
	PlayerInfoMock playermock(0,0,0,0); \
    TrackPositionMock trackpositionmock(false, 0, 0, 0, 0); \
	CarInfoMock carinfomock(0, 0, 0, 0, trackpositionmock); \
    EnvironmentInfoMock environmentmock(0, 0, 0);           \
	DriveSituationMock mock(playermock,carinfomock,environmentmock, 0);

class DriveSituationMock
{
public:
    DriveSituationMock(PlayerInfoMock m_PlayerInfo, CarInfoMock mCarInfo, EnvironmentInfoMock mEnvironmentInfo, unsigned long mTickCount)
            : m_playerInfo(m_PlayerInfo), m_carInfo(mCarInfo), m_environmentInfo(mEnvironmentInfo), m_tickCount(mTickCount) {};

    PlayerInfoMock m_playerInfo;
    EnvironmentInfoMock m_environmentInfo;
    CarInfoMock m_carInfo;
    unsigned long m_tickCount;

    PlayerInfoMock* GetPlayerInfo(){return &m_playerInfo;}
    EnvironmentInfoMock* GetEnvironmentInfo(){return &m_environmentInfo;};
    CarInfoMock* GetCarInfo(){return &m_carInfo;};
    unsigned long GetTickCount(){return m_tickCount;};
};

/// @brief  A sample drive situation for testing
inline DriveSituationMock GetExampleDriveSituation()
{
    TrackPositionMock trackpos(true, 0.1, 0.2, 0.3, 0.4);
    CarInfoMock carinfo(1.1, 1.2, 1, true, trackpos);
    PlayerInfoMock playerinfo(2.1, 2.2, 2.3, 2.4);
    EnvironmentInfoMock environmentinfo(3, 4, 5);
    return { playerinfo, carinfo, environmentinfo, 69 };
}