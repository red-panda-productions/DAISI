#pragma once


class TrackPosition
{
public:
    TrackPosition(bool p_offroad, float p_toStart, float p_toRight, float p_toMiddle, float p_toLeft);
    bool Offroad();
    float ToStart();
    float ToRight();
    float ToMiddle();
    float ToLeft();
private:
    bool m_offroad;
    float m_toStart;
    float m_toRight;
    float m_toMiddle;
    float m_toLeft;
};