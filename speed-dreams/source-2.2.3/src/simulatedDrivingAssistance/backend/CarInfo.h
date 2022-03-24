#pragma once
#include "TrackPosition.h"

/// @brief Represents all info that can be gathered from the in game car
class CarInfo
{
public:
    CarInfo(TrackPosition p_position, float p_speed, float p_topSpeed, int p_gear, bool p_headlights);

    float Speed() const;
    float TopSpeed() const;
    int Gear() const;
    bool Headlights() const;

    const TrackPosition* TrackLocalPosition() const;
private:
    TrackPosition m_trackPosition;
    float m_speed;
    float m_topSpeed;
    int m_gear;
    bool m_headlights;
};