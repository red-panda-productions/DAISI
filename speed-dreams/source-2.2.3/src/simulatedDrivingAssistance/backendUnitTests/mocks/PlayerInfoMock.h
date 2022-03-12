#pragma once

class PlayerInfoMock
{
public:
    PlayerInfoMock(float p_steerCmd, float p_accelCmd, float p_brakeCmd, float p_clutchCmd)
    {
        m_steerCmd = p_steerCmd;
        m_accelCmd = p_accelCmd;
        m_brakeCmd = p_brakeCmd;
        m_clutchCmd = p_clutchCmd;
    }

    float m_steerCmd;
    float m_accelCmd;
    float m_brakeCmd;
    float m_clutchCmd;

    float SteerCmd(){return m_steerCmd;};
    float AccelCmd(){return m_accelCmd;};
    float BrakeCmd(){return m_brakeCmd;};
    float ClutchCmd(){return m_clutchCmd;};
};