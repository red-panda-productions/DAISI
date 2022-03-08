#pragma once

class CarInfoMock
{
public:
    CarInfoMock(float p_speed,float p_topSpeed,float p_steerCmd,float p_accelCmd,float p_brakeCmd,float p_clutchCmd)
    {
        m_speed = p_speed;
        m_topSpeed = p_topSpeed;
        m_steerCmd = p_steerCmd;
        m_accelCmd = p_accelCmd;
        m_brakeCmd = p_brakeCmd;
        m_clutchCmd = p_clutchCmd;
    }

    float m_speed;
    float m_topSpeed;
    float m_steerCmd;
    float m_accelCmd;
    float m_brakeCmd;
    float m_clutchCmd;

    float Speed(){return m_speed;};
    float TopSpeed(){return m_topSpeed;};
    float SteerCmd(){return m_steerCmd;};
    float AccelCmd(){return m_accelCmd;};
    float BrakeCmd(){return m_brakeCmd;};
    float ClutchCmd(){return m_clutchCmd;};
};