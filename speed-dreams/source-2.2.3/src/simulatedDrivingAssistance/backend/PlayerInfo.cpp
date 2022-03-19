#include "PlayerInfo.h"

PlayerInfo::PlayerInfo(float p_steer, float p_accel, float p_brake, float p_clutch)
{
	m_steer = p_steer;
	m_accel = p_accel;
	m_brake = p_brake;
	m_clutch = p_clutch;
}

float PlayerInfo::SteerCmd()
{
	return m_steer;
}

float PlayerInfo::AccelCmd()
{
	return m_accel;
}

float PlayerInfo::BrakeCmd()
{
	return m_brake;
}

float PlayerInfo::ClutchCmd()
{
	return m_clutch;
}



