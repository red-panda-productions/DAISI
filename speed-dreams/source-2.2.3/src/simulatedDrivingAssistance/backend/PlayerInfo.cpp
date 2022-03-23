#include "PlayerInfo.h"

/// @brief			 Constructs the playerInfo
/// @param  p_steer  The steering action
/// @param  p_accel  The acceleration action
/// @param  p_brake  The brake action
/// @param  p_clutch The Clutch action
PlayerInfo::PlayerInfo(float p_steer, float p_accel, float p_brake, float p_clutch)
{
	m_steer = p_steer;
	m_accel = p_accel;
	m_brake = p_brake;
	m_clutch = p_clutch;
}

/// @brief  Gets the steering amount
/// @return The steering amount
float PlayerInfo::SteerCmd() const
{
	return m_steer;
}

/// @brief  Gets the acceleration amount
/// @return The acceleration amount
float PlayerInfo::AccelCmd() const
{
	return m_accel;
}

/// @brief  Gets the brake amount
/// @return The brake amount
float PlayerInfo::BrakeCmd() const
{
	return m_brake;
}

/// @brief  Gets the clutch amount
/// @return The clutch amount
float PlayerInfo::ClutchCmd() const
{
	return m_clutch;
}



