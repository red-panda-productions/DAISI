#include <gtest/gtest.h>
#include "SocketBlackBox.h"
#include "SocketBlackBox.cpp"
#include "mocks/DriveSituationMock.h"
#include <random>
#include <limits>
#include "Random.hpp"

/// create all variables of drive situation and assign random values
#define RANDOM_VALUE_ASSIGNMENT \
    Random random;\
    float randomAccelCmd = random.NextFloat(-1000,1000);\
    float randomBrakeCmd = random.NextFloat(-1000,1000);\
    float randomClutchCmd = random.NextFloat(-1000,1000);\
    float randomSteerCmd = random.NextFloat(-1000,1000);\
    float randomSpeed = random.NextFloat(-1000,1000);\
    float randomTopSpeed = random.NextFloat(-1000,1000);\
    int randomTimeOfDay = random.NextInt(-1000,1000);\
    int randomClouds = random.NextInt(-1000,1000);\
    bool randomOffroad = random.NextBool();\
    float randomToMiddle = random.NextFloat(-1000,1000);\
    float randomToLeft = random.NextFloat(-1000,1000);\
    float randomToRight = random.NextFloat(-1000,1000);\
    float randomToStart = random.NextFloat(-1000,1000);\
    float randomTimeLastSteer = random.NextFloat(-1000,1000);

/// create a drive situation mock and insert all variables
#define DEFINE_DRIVE_MOCK \
    DriveSituationMock driveSituation(\
        PlayerInfoMock(randomTimeLastSteer),\
        CarInfoMock(randomSpeed, randomTopSpeed, randomSteerCmd,randomAccelCmd,randomBrakeCmd,randomClutchCmd),\
        EnvironmentInfoMock(randomOffroad,randomTimeOfDay, randomClouds,\
            TrackPositionMock(randomToStart,randomToRight,randomToMiddle,randomToLeft)))

/// create a socket black box and initialize the maps.
#define SETUP_SOCKET \
    SocketBlackBox<DriveSituationMock> socketBlackBox;\
    socketBlackBox.Initialize()

/// <summary>
/// Tests if all variables can be serialized correctly
/// </summary>
TEST(MsgpackSerializeTests, SerializeAll)
{
    for (int test = 0; test < 100; test++) {
        RANDOM_VALUE_ASSIGNMENT;
        DEFINE_DRIVE_MOCK;
        SETUP_SOCKET;

        std::vector<std::string> controlVector{
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
        socketBlackBox.m_variablesToSend = std::vector<std::string>{
            "AccelCmd",
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
        msgpack::object const &obj = result.get();

        std::vector<std::string> testValues;
        obj.convert(testValues);

        int i = 0;
        for (auto it = testValues.begin(); it != testValues.end(); it++) {
            //std::cout << *it << " should equal: " << controlVector.at(i) << std::endl;
            ASSERT_EQ(*it, controlVector.at(i++));
        }
    }
}


/// <summary>
/// Tests if serialization also works if not all variables are included.
/// </summary>
TEST(MsgpackSerializeTests, SerializeSome)
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
    socketBlackBox.m_variablesToSend = std::vector<std::string>{
        "AccelCmd",
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

/// <summary>
/// Tests if all variables are correctly serialized, even when a variable that does not exist is tried.
/// </summary>
TEST(MsgpackSerializeTests, NonExistingVariableKey)
{
    RANDOM_VALUE_ASSIGNMENT;
    DEFINE_DRIVE_MOCK;
    SETUP_SOCKET;

    std::vector<std::string> controlVector{
        std::to_string(randomSpeed),
        std::to_string(randomToMiddle),
        std::to_string(randomOffroad)
    };

    // should be the same order as control vector
    socketBlackBox.m_variablesToSend = std::vector<std::string>{
        "Speed",
        "NON_EXISTING_VARIABLE",
        "Offroad"};

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
        if (i == 1) ASSERT_EQ(testValues.at(1), "Variable key does not exist");
        else ASSERT_EQ(*it, controlVector.at(i++));
    }

}

/// <summary>
/// Tests if an exception is thrown when the variablesToSend vector is empty.
/// Should also serialize an error message for the black box.
/// </summary>
TEST(MsgpackSerializeTests, EmptyVariableVector)
{
    RANDOM_VALUE_ASSIGNMENT;
    DEFINE_DRIVE_MOCK;
    SETUP_SOCKET;

    msgpack::sbuffer sbuffer;
    ASSERT_THROW(socketBlackBox.SerializeDriveSituation(sbuffer, driveSituation), std::exception);

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