#include "DriveSituation.h"

DriveSituation::DriveSituation() :
	m_environmentInfo(0,0,0),
	m_carInfo(
		TrackPosition(0,0,0,0,0)
		,0,0,0,false),
	m_playerInfo(0,0,0,0),
    m_currentTime(0)
{
}

DriveSituation::DriveSituation(EnvironmentInfo p_environmentInfo, CarInfo p_carInfo, PlayerInfo p_playerInfo, double p_currentTime) :
	m_environmentInfo(p_environmentInfo),
	m_carInfo(p_carInfo),
	m_playerInfo(p_playerInfo),
    m_currentTime(p_currentTime)
{
}

EnvironmentInfo* DriveSituation::GetEnvironmentInfo()
{
    return &m_environmentInfo;
};

CarInfo* DriveSituation::GetCarInfo()
{
    return &m_carInfo;
};

PlayerInfo* DriveSituation::GetPlayerInfo()
{
    return &m_playerInfo;
}

double DriveSituation::GetTime()
{
    return m_currentTime;
}