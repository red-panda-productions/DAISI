#include "EnvironmentInfo.h"

EnvironmentInfo::EnvironmentInfo(int p_timeOfDay, int p_clouds, int p_rain)
{
	m_timeOfDay = p_timeOfDay;
	m_clouds = p_clouds;
	m_rain = p_rain;
}

int EnvironmentInfo::TimeOfDay()
{
	return m_timeOfDay;
}

int EnvironmentInfo::Clouds()
{
	return m_clouds;
}

int EnvironmentInfo::Rain()
{
	return m_rain;
}

void EnvironmentInfo::SetTimeOfDay(int p_timeOfDay)
{
	m_timeOfDay = p_timeOfDay;
}

void EnvironmentInfo::SetClouds(int p_clouds)
{
	m_clouds = p_clouds;
}

void EnvironmentInfo::SetRain(int p_rain)
{
	m_rain = p_rain;
}
