#pragma once
class EnvironmentInfo
{
public:
	EnvironmentInfo(int p_timeOfDay, int p_clouds, int p_rain);
	int TimeOfDay();
	int Clouds();
	int Rain();
private:
	int m_timeOfDay;
	int m_clouds;
	int m_rain;
};