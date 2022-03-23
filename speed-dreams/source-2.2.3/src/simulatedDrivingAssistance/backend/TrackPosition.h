#pragma once

/// @brief A class that represents all data that can be gathered from the track position
class TrackPosition
{
public:
    TrackPosition(bool p_offroad, float p_toStart, float p_toRight, float p_toMiddle, float p_toLeft);
    bool Offroad() const;
    float ToStart() const;
    float ToRight() const;
    float ToMiddle() const;
    float ToLeft() const;
private:
    bool m_offroad;
    float m_toStart;
    float m_toRight;
    float m_toMiddle;
    float m_toLeft;
};