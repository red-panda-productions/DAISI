#pragma once

#include "TrackPositionMock.h"

class CarInfoMock
{
public:
    CarInfoMock(float p_speed, float p_topSpeed, int p_gear, bool p_headlights, TrackPositionMock trackPosition)
            : trackPosition(trackPosition) {
        m_speed = p_speed;
        m_topSpeed = p_topSpeed;
        m_gear = p_gear;
        m_headlights = p_headlights;
    }

    TrackPositionMock trackPosition;
    float m_speed;
    float m_topSpeed;
    int m_gear;
    bool m_headlights;

    float Speed(){return m_speed;};
    float TopSpeed(){return m_topSpeed;};
    int Gear(){return m_gear;};
    bool Headlights(){return m_headlights;};
    TrackPositionMock TrackLocalPosition() {return trackPosition;}
};