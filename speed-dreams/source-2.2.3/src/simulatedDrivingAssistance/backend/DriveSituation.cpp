#include "DriveSituation.h"

EnvironmentInfo* DriveSituation::GetEnvironmentInfo()
{
    return &m_environmentInfo;
};

CarInfo* DriveSituation::GetCarInfo()
{
    return &m_carInfo;
};

PlayerInfo* DriveSituation::GetPlayerInfo()
{
    return &m_playerInfo;
}