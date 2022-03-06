#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "Utils.h"
#include "SocketBlackBox.h"
#include "SocketBlackBox.cpp"
#include "PlayerInfo.h"
#include "DriveSituation.h"
#include <random>
#include <limits>

#define RANDOM_VALUE_ASSIGNMENT std::random_device rd;\
                                std::default_random_engine eng(rd());\
                                std::uniform_real_distribution<> distr(-1000 , 1000);\
                                float randomAccelCmd = distr(eng);\
                                float randomBrakeCmd = distr(eng);\
                                float randomClutchCmd = distr(eng);\
                                float randomSteerCmd = distr(eng);\
                                float randomSpeed = distr(eng);\
                                float randomTopSpeed = distr(eng);\
                                int randomTimeOfDay = (int)distr(eng);\
                                int randomClouds = (int)distr(eng);\
                                bool randomOffroad = std::uniform_real_distribution<>(0,1)(eng) < 0.5f? false : true;\
                                float randomToMiddle = distr(eng);\
                                float randomToLeft = distr(eng);\
                                float randomToRight = distr(eng);\
                                float randomToStart = distr(eng);\
                                float randomTimeLastSteer = distr(eng);

#define DEFINE_DRIVE_MOCK       DriveSituationMock driveSituation(\
                                        PlayerInfoMock(randomTimeLastSteer),\
                                        CarInfoMock(randomSpeed, randomTopSpeed, randomSteerCmd,randomAccelCmd,randomBrakeCmd,randomClutchCmd),\
                                        EnvironmentInfoMock(randomOffroad,randomTimeOfDay, randomClouds,\
                                                            TrackPositionMock(randomToStart,randomToRight,randomToMiddle,randomToLeft)));

#define SETUP_SOCKET            SocketBlackBox<DriveSituationMock> socketBlackBox;\
                                socketBlackBox.Initialize();

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

class EnvironmentInfoMock
{
public:
    EnvironmentInfoMock(bool p_offroad, int p_timeOfDay, int p_clouds,
                        TrackPositionMock trackPosition) : trackPosition(trackPosition)
    {
        m_offroad = p_offroad;
        m_timeOfDay = p_timeOfDay;
        m_clouds = p_clouds;
    }

    bool m_offroad;
    int m_timeOfDay;
    int m_clouds;
    TrackPositionMock trackPosition;

    bool Offroad(){return m_offroad;};
    int TimeOfDay(){return m_timeOfDay;};
    int Clouds(){return m_clouds;};
    TrackPositionMock TrackLocalPosition() {return trackPosition;}
};

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

class DriveSituationMock
{
public:
    DriveSituationMock(PlayerInfoMock m_PlayerInfo, CarInfoMock mCarInfo, EnvironmentInfoMock mEnvironmentInfo)
            : m_playerInfo(m_PlayerInfo), m_carInfo(mCarInfo), m_environmentInfo(mEnvironmentInfo) {};

    PlayerInfoMock m_playerInfo;
    EnvironmentInfoMock m_environmentInfo;
    CarInfoMock m_carInfo;

    PlayerInfoMock GetPlayerInfo(){return m_playerInfo;}
    EnvironmentInfoMock GetEnvironmentInfo(){return m_environmentInfo;};
    CarInfoMock GetCarInfo(){return m_carInfo;};
};

TEST(MsgpackTests, SerializeAll)
{
    RANDOM_VALUE_ASSIGNMENT;
    DEFINE_DRIVE_MOCK;
    SETUP_SOCKET;

    std::vector<std::string> controlVector {
                                std::to_string(randomAccelCmd),
                                std::to_string(randomBrakeCmd),
                                std::to_string(randomClutchCmd),
                                std::to_string(randomSteerCmd),
                                std::to_string(randomSpeed),
                                std::to_string(randomTopSpeed),
                                std::to_string(randomTimeOfDay),
                                std::to_string(randomClouds),
                                std::to_string(randomOffroad),
                                std::to_string(randomToMiddle),
                                std::to_string(randomToLeft),
                                std::to_string(randomToRight),
                                std::to_string(randomToStart),
                                std::to_string(randomTimeLastSteer)
                                };

    // should be in the same order as the control vector
    socketBlackBox.m_variablesToSend = std::vector<std::string>{"AccelCmd",
                                                                "BrakeCmd",
                                                                "ClutchCmd",
                                                                "SteerCmd",
                                                                "Speed",
                                                                "TopSpeed",
                                                                "TimeOfDay",
                                                                "Clouds",
                                                                "Offroad",
                                                                "ToMiddle",
                                                                "ToLeft",
                                                                "ToRight",
                                                                "ToStart",
                                                                "TimeLastSteer"};

    msgpack::sbuffer sbuffer;
    socketBlackBox.SerializeDriveSituation(sbuffer, driveSituation);

    msgpack::unpacker unpacker;
    unpacker.reserve_buffer(sbuffer.size());
    std::memcpy(unpacker.buffer(), sbuffer.data(), sbuffer.size());
    unpacker.buffer_consumed(sbuffer.size());
    msgpack::unpacked result;
    unpacker.next(result);
    msgpack::object const& obj = result.get();

    std::vector<std::string> testValues;
    obj.convert(testValues);

    int i = 0;
    for (auto it = testValues.begin(); it != testValues.end(); it++)
    {
        //std::cout << *it << " should equal: " << controlVector.at(i) << std::endl;
        ASSERT_EQ(*it, controlVector.at(i++));
    }
}

TEST(MsgpackTests, SerializeSome)
{
    RANDOM_VALUE_ASSIGNMENT;
    DEFINE_DRIVE_MOCK;
    SETUP_SOCKET;

    std::vector<std::string> controlVector {
            std::to_string(randomAccelCmd),
            std::to_string(randomClutchCmd),
            std::to_string(randomSpeed),
            std::to_string(randomTimeOfDay),
            std::to_string(randomOffroad),
            std::to_string(randomToLeft),
            std::to_string(randomTimeLastSteer)
    };

    // should be in the same order as the control vector
    socketBlackBox.m_variablesToSend = std::vector<std::string>{"AccelCmd",
                                                                "ClutchCmd",
                                                                "Speed",
                                                                "TimeOfDay",
                                                                "Offroad",
                                                                "ToLeft",
                                                                "TimeLastSteer"};

    msgpack::sbuffer sbuffer;
    socketBlackBox.SerializeDriveSituation(sbuffer, driveSituation);

    msgpack::unpacker unpacker;
    unpacker.reserve_buffer(sbuffer.size());
    std::memcpy(unpacker.buffer(), sbuffer.data(), sbuffer.size());
    unpacker.buffer_consumed(sbuffer.size());
    msgpack::unpacked result;
    unpacker.next(result);
    msgpack::object const& obj = result.get();

    std::vector<std::string> testValues;
    obj.convert(testValues);

    int i = 0;
    for (auto it = testValues.begin(); it != testValues.end(); it++)
    {
        //std::cout << *it << " should equal: " << controlVector.at(i) << std::endl;
        ASSERT_EQ(*it, controlVector.at(i++));
    }
}

TEST(MsgpackTests, NonExistingVariableKey)
{
    RANDOM_VALUE_ASSIGNMENT;
    DEFINE_DRIVE_MOCK;
    SETUP_SOCKET

    socketBlackBox.m_variablesToSend = std::vector<std::string>{"NON_EXISTING_VARIABLE"};

    msgpack::sbuffer sbuffer;
    socketBlackBox.SerializeDriveSituation(sbuffer, driveSituation);

    msgpack::unpacker unpacker;
    unpacker.reserve_buffer(sbuffer.size());
    std::memcpy(unpacker.buffer(), sbuffer.data(), sbuffer.size());
    unpacker.buffer_consumed(sbuffer.size());
    msgpack::unpacked result;
    unpacker.next(result);
    msgpack::object const& obj = result.get();

    std::vector<std::string> testValues;
    obj.convert(testValues);

    ASSERT_EQ(testValues.at(0), "Variable key does not exist");
}

TEST(MsgpackTests, NoVariableVector)
{
    RANDOM_VALUE_ASSIGNMENT;
    DEFINE_DRIVE_MOCK;
    SETUP_SOCKET

    msgpack::sbuffer sbuffer;
    socketBlackBox.SerializeDriveSituation(sbuffer, driveSituation);

    msgpack::unpacker unpacker;
    unpacker.reserve_buffer(sbuffer.size());
    std::memcpy(unpacker.buffer(), sbuffer.data(), sbuffer.size());
    unpacker.buffer_consumed(sbuffer.size());
    msgpack::unpacked result;
    unpacker.next(result);
    msgpack::object const& obj = result.get();

    std::vector<std::string> testValues;
    obj.convert(testValues);

    ASSERT_EQ(testValues.at(0), "There are no variables to send");
}