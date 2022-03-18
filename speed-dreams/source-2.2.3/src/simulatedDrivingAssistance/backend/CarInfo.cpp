#include "CarInfo.h"

CarInfo::CarInfo(TrackPosition& p_position, float p_speed, float p_topSpeed, int p_gear, bool p_headlights) : m_trackPosition(p_position)
{
	m_speed = p_speed;
	m_topSpeed = p_topSpeed;
	m_gear = p_gear;
	m_headlights = p_headlights;
}

float CarInfo::Speed()
{
	return m_speed;
}

float CarInfo::TopSpeed()
{
	return m_topSpeed;
}

int CarInfo::Gear()
{
	return m_gear;
}

bool CarInfo::Headlights()
{
	return m_headlights;
}

TrackPosition* CarInfo::TrackLocalPosition()
{
    return &m_trackPosition;
}
