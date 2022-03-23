#include "EnvironmentInfo.h"

/// @brief			    Constructs the environment
/// @param  p_timeOfDay The time of day
/// @param  p_clouds    The clouds 
/// @param  p_rain      If there is rain
EnvironmentInfo::EnvironmentInfo(int p_timeOfDay, int p_clouds, int p_rain)
{
	m_timeOfDay = p_timeOfDay;
	m_clouds = p_clouds;
	m_rain = p_rain;
}

/// @brief  Gets the time of the day
/// @return The time of day
int EnvironmentInfo::TimeOfDay() const
{
	return m_timeOfDay;
}

/// @brief  Gets the clouds
/// @return The clouds
int EnvironmentInfo::Clouds() const
{
	return m_clouds;
}

/// @brief  Gets the rain
/// @return If it rains
int EnvironmentInfo::Rain() const
{
	return m_rain;
}

/// @brief              Sets the time of day
/// @param  p_timeOfDay The time of day
void EnvironmentInfo::SetTimeOfDay(int p_timeOfDay)
{
	m_timeOfDay = p_timeOfDay;
}

/// @brief			 Sets the clouds
/// @param  p_clouds The clouds
void EnvironmentInfo::SetClouds(int p_clouds)
{
	m_clouds = p_clouds;
}

/// @brief		   Sets the rain
/// @param  p_rain The rain
void EnvironmentInfo::SetRain(int p_rain)
{
	m_rain = p_rain;
}
