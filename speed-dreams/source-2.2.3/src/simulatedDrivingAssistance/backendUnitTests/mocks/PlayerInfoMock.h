#pragma once

class PlayerInfoMock
{
public:
    PlayerInfoMock(float p_timeLastSteer)
    {
        m_timeLastSteer = p_timeLastSteer;
    }

    float m_timeLastSteer;

    float TimeLastSteer(){return m_timeLastSteer;}
};