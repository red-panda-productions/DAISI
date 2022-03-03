#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "Utils.h"
#include "SocketBlackBox.h"
#include "PlayerInfo.h"
#include "DriveSituation.h"


class PlayerInfoMock : PlayerInfo
{
public:
    PlayerInfoMock(float p_testValue)
    {
        m_timeLastSteerValue = p_testValue;
    }

    float m_timeLastSteerValue;

    float TimeLastSteer()
    {
        return m_timeLastSteerValue;
    }
};

class DriveSituationMock : public DriveSituation
{
public:
    explicit DriveSituationMock(PlayerInfoMock m_PlayerInfo) : m_playerInfo(m_PlayerInfo){};

    PlayerInfoMock m_playerInfo;

    PlayerInfoMock GetPlayerInfo()
    {
        return m_playerInfo;
    }

    //MOCK_METHOD(PlayerInfo ,GetPlayerInfo, ());
};



TEST(MsgpackTests, Example)
{
	ASSERT_DURATION_LE(2, while (true){});
}

TEST(MsgpackTests, SerializeNormal)
{
    SocketBlackBox<DriveSituationMock> socketBlackBox;
    socketBlackBox.Initialize();

    PlayerInfoMock playerInfoMock(3);

    DriveSituationMock driveSituation(playerInfoMock);

    socketBlackBox.m_variablesToSend = std::vector<std::string>{"Speed", "AccelCmd"};

    socketBlackBox.GetDecisions(driveSituation);


    ASSERT_DURATION_LE(2, while (true){});
}