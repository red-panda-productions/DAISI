#pragma once
class EnvironmentInfo
{
public:
	EnvironmentInfo(int p_timeOfDay, int p_clouds, int p_rain);
	int TimeOfDay();
	int Clouds();
	int Rain();
	void SetTimeOfDay(int p_timeOfDay);
	void SetClouds(int p_clouds);
	void SetRain(int p_rain);
private:
	int m_timeOfDay;
	int m_clouds;
	int m_rain;
};