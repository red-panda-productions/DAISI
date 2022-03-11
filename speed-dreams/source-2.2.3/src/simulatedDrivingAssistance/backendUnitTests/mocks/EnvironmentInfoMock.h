#pragma once
#include "TrackPositionMock.h"

class EnvironmentInfoMock
{
public:
    EnvironmentInfoMock(bool p_offroad, int p_timeOfDay, int p_clouds,
                        TrackPositionMock trackPosition) : trackPosition(trackPosition)
    {
        m_offroad = p_offroad;
        m_timeOfDay = p_timeOfDay;
        m_clouds = p_clouds;
    }

    bool m_offroad;
    int m_timeOfDay;
    int m_clouds;
    TrackPositionMock trackPosition;

    bool Offroad(){return m_offroad;};
    int TimeOfDay(){return m_timeOfDay;};
    int Clouds(){return m_clouds;};
    TrackPositionMock TrackLocalPosition() {return trackPosition;}
};