#pragma once

/// @brief Represents all info that can be gathered from an environment
class EnvironmentInfo
{
public:
	EnvironmentInfo(int p_timeOfDay, int p_clouds, int p_rain);
	int TimeOfDay() const;
	int Clouds() const;
	int Rain() const;
	void SetTimeOfDay(int p_timeOfDay);
	void SetClouds(int p_clouds);
	void SetRain(int p_rain);
private:
	int m_timeOfDay;
	int m_clouds;
	int m_rain;
};