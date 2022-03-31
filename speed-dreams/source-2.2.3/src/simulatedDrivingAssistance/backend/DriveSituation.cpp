#include "DriveSituation.h"

/// @brief Default constructor of drive situation, where everything is 0
DriveSituation::DriveSituation() :
	m_environmentInfo(0,0,0),
	m_carInfo(
		TrackPosition(0,0,0,0,0)
		,0,0,0,false),
	m_playerInfo(0,0,0,0),
    m_tickCount(0)
{
}

/// @brief					  Constructs the current drive situation
/// @param  p_environmentInfo The info of the environment
/// @param  p_carInfo		  The info of the car
/// @param  p_playerInfo      The info of the player
/// @param  p_tickCount       The current time of the game in ticks
DriveSituation::DriveSituation(EnvironmentInfo p_environmentInfo, CarInfo p_carInfo, PlayerInfo p_playerInfo, unsigned long p_tickCount) :
	m_environmentInfo(p_environmentInfo),
	m_carInfo(p_carInfo),
	m_playerInfo(p_playerInfo),
    m_tickCount(p_tickCount)
{
}

/// @brief  Gets the environment info
/// @return The environment info
const EnvironmentInfo* DriveSituation::GetEnvironmentInfo() const
{
    return &m_environmentInfo;
};

/// @brief  Gets the car info
/// @return The car info
const CarInfo* DriveSituation::GetCarInfo() const
{
    return &m_carInfo;
};

/// @brief  Gets the player info
/// @return The player info
const PlayerInfo* DriveSituation::GetPlayerInfo() const
{
    return &m_playerInfo;
}

/// @brief  Gets the current time
/// @return The current time
unsigned long DriveSituation::GetTickCount() const
{
    return m_tickCount;
}