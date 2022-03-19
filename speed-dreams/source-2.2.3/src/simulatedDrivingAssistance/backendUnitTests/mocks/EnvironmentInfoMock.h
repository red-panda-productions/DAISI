#pragma once
#include "TrackPositionMock.h"

class EnvironmentInfoMock
{
public:
    EnvironmentInfoMock(int p_timeOfDay, int p_clouds, int p_rain)
    {
        m_timeOfDay = p_timeOfDay;
        m_clouds = p_clouds;
        m_rain = p_rain;
    }

    int m_timeOfDay;
    int m_clouds;
    int m_rain;

    int TimeOfDay(){return m_timeOfDay;};
    int Clouds(){return m_clouds;};
    int Rain(){return m_rain;};
};