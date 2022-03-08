#pragma once

class TrackPositionMock
{
public:
    TrackPositionMock(float p_toStart, float p_toRight, float p_toMiddle, float p_toLeft)
    {
        m_toStart = p_toStart;
        m_toRight = p_toRight;
        m_toMiddle = p_toMiddle;
        m_toLeft = p_toLeft;
    }

    float m_toStart;
    float m_toRight;
    float m_toMiddle;
    float m_toLeft;

    float ToStart(){return m_toStart;};
    float ToRight(){return m_toRight;};
    float ToMiddle(){return m_toMiddle;};
    float ToLeft(){return m_toLeft;};
};