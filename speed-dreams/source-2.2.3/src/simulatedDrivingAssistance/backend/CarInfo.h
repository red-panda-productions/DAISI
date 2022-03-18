#pragma once

#include "TrackPosition.h"
class CarInfo
{
public:
    CarInfo(TrackPosition& p_position, float p_speed, float p_topSpeed, int p_gear, bool p_headlights);

    float Speed();
    float TopSpeed();
    int Gear();
    bool Headlights();

    TrackPosition* TrackLocalPosition();
private:
    TrackPosition m_trackPosition;
    float m_speed;
    float m_topSpeed;
    int m_gear;
    bool m_headlights;
};