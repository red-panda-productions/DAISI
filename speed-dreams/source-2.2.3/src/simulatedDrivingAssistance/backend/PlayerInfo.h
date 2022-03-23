#pragma once

class PlayerInfo
{
public:
    PlayerInfo(float p_steer, float p_accel, float p_brake, float p_clutch);

    float SteerCmd() const;
    float AccelCmd() const;
    float BrakeCmd() const;
    float ClutchCmd() const;
private:
    float m_steer;
    float m_accel;
    float m_brake;
    float m_clutch;
};