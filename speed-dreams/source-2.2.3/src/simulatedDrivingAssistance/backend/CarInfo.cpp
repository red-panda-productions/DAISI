#include "CarInfo.h"

/// @brief			     Constructs all info of the car
/// @param  p_position   The position
/// @param  p_speed		 The speed
/// @param  p_topSpeed	 The topSpeed
/// @param  p_gear       The gear
/// @param  p_headlights The lights
CarInfo::CarInfo(TrackPosition p_position, float p_speed, float p_topSpeed, int p_gear, bool p_headlights) : m_trackPosition(p_position)
{
	m_speed = p_speed;
	m_topSpeed = p_topSpeed;
	m_gear = p_gear;
	m_headlights = p_headlights;
}

/// @brief  Gets the speed of the car
/// @return The speed of the car
float CarInfo::Speed() const
{
	return m_speed;
}

/// @brief  Gets the top speed of the car
/// @return The top speed of the car
float CarInfo::TopSpeed() const
{
	return m_topSpeed;
}

/// @brief  Gets the gear of the car
/// @return The gear of the car
int CarInfo::Gear() const
{
	return m_gear;
}

/// @brief  Gets the lights of the car
/// @return The lights of the car
bool CarInfo::Headlights() const 
{
	return m_headlights;
}

/// @brief  Gets the local position of the car
/// @return The local position of the car
const TrackPosition* CarInfo::TrackLocalPosition() const 
{
    return &m_trackPosition;
}
