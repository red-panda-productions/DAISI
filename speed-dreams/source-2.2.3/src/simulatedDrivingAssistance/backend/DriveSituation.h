#pragma once
#include "EnvironmentInfo.h"
#include "CarInfo.h"
#include "PlayerInfo.h"

/// @brief Represents the drive situation at a timestamp
class DriveSituation
{
public:
    DriveSituation();
    DriveSituation(EnvironmentInfo p_environmentInfo, CarInfo p_carInfo, PlayerInfo p_playerInfo, double p_currentTime);
    const EnvironmentInfo* GetEnvironmentInfo() const;
    const CarInfo* GetCarInfo() const;
    const PlayerInfo* GetPlayerInfo() const;
    double GetTime() const;

private:
    EnvironmentInfo m_environmentInfo;
    CarInfo m_carInfo;
    PlayerInfo m_playerInfo;
    double m_currentTime;
};