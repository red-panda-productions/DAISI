/*
 * This program has been developed by students from the bachelor Computer Science at
 * Utrecht University within the Software Project course.
 * © Copyright Utrecht University (Department of Information and Computing Sciences)
 */

#pragma once
#include <gtest/gtest.h>
#include <limits>
#include <thread>
#include "ClientSocket.h"
#include "DecisionTuple.h"
#include "msgpack.hpp"
#include "SocketBlackBox.h"
#include "SocketBlackBox.inl"
#include "TestUtils.h"
#include "mocks/PointerManagerMock.h"
#include "mocks/BlackBoxDataMock.h"
#include "Mediator.h"
#include "SDAConfig.h"
#include "mocks/DecisionMakerMock.h"

/// @brief A mediator that uses the standard SDecisionMakerMock
#define MockMediator Mediator<SDecisionMakerMock>

/// @brief A mediator that uses SDAConfig in DecisionmakerMock internally
#define SDAConfigMediator Mediator<DecisionMakerMock<SDAConfig>>

#define TEST_BUFFER_SIZE 512
#define TOLERANCE        0.1f
#define STEER_VALUE      1.0f
#define BRAKE_VALUE      2.0f

/// @brief				Sets up the connection between the AI and the test
/// @param  method_name The method that needs to be tested
#define SETUP(method_name)                                                               \
    std::thread t = std::thread(method_name);                                            \
    t.detach();                                                                          \
    ClientSocket client;                                                                 \
    ASSERT_DURATION_LE(                                                                  \
        2, while (client.Initialize() != IPCLIB_SUCCEED) { std::this_thread::yield(); }) \
    ASSERT_EQ(client.SendData("AI ACTIVE", 9), IPCLIB_SUCCEED);                          \
    char buffer[TEST_BUFFER_SIZE];                                                       \
    ASSERT_EQ(client.AwaitData(buffer, TEST_BUFFER_SIZE), IPCLIB_SUCCEED);               \
    ASSERT_TRUE(buffer[0] == 'O' && buffer[1] == 'K');

/// @brief The black box side of the test, as these tests have to run in parallel
void BlackBoxSideAsync()
{
    SocketBlackBox<BlackBoxDataMock, PointerManagerMock> bb;
    Random random;
    BlackBoxDataMock mock = CreateRandomBlackBoxDataMock(random);
    BlackBoxDataMock exampleSituation = GetExampleBlackBoxDataMock();
    BlackBoxDataMock situations[2]{mock, exampleSituation};

    /// intializes the black box with 2 tests and async
    bb.Initialize(true, mock, situations, 2);
    DecisionTuple decisions;

    tCarElt car;
    tSituation situation;

    tCar* carTable = new tCar[1];

    ASSERT_DURATION_LE(
        1, while (!bb.GetDecisions(carTable, &car, &situation, 0, decisions)) { std::this_thread::yield(); })

    // check the result
    ASSERT_ALMOST_EQ(decisions.GetSteerAmount(), STEER_VALUE, TOLERANCE);
    ASSERT_ALMOST_EQ(decisions.GetBrakeAmount(), BRAKE_VALUE, TOLERANCE);

    ASSERT_FALSE(bb.GetDecisions(carTable, &car, &situation, 0, decisions));  // to check whether the async function can return false

    // shut the server down
    bb.Shutdown();
}

void BlackBoxSideSync()
{
    SocketBlackBox<BlackBoxDataMock, PointerManagerMock> bb;
    Random random;
    BlackBoxDataMock mock = CreateRandomBlackBoxDataMock(random);
    BlackBoxDataMock exampleSituation = GetExampleBlackBoxDataMock();
    BlackBoxDataMock situations[2]{mock, exampleSituation};

    /// intializes the black box with 2 tests and sync connection
    bb.Initialize(false, mock, situations, 2);
    DecisionTuple decisions;

    tCarElt car;
    tSituation situation;

    tCar* carTable = new tCar[1];

    // It should always return true because it waits for a decision
    ASSERT_TRUE(bb.GetDecisions(carTable, &car, &situation, 0, decisions));

    // check the result
    ASSERT_ALMOST_EQ(decisions.GetSteerAmount(), STEER_VALUE, TOLERANCE);
    ASSERT_ALMOST_EQ(decisions.GetBrakeAmount(), BRAKE_VALUE, TOLERANCE);

    // shut the server down
    bb.Shutdown();
}

/// @brief					 Tests if the parsed drivesituation is the same as the target
/// @param  p_driveSituation Parsed drive situation
/// @param  p_target		 The target
void TestDriveSituation(std::vector<std::string>& p_driveSituation, BlackBoxDataMock p_target)
{
}

/// @brief Tests an entire run of the framework
/// @param p_blackboxFunction function with blackbox initialisation and behaviour
/// @param p_async whether or not the connection is async
void SocketTest(void (*p_blackboxFunction)(), bool p_async)
{
    SDAConfigMediator::ClearInstance();
    ASSERT_TRUE(SetupSingletonsFolder());

    // creates a connection between the black box and a client
    SETUP(p_blackboxFunction)

    std::vector<std::string> order = {
        "ACTIONORDER",
        "Steer",
        "Brake"};

    // sends required and sending data of client
    msgpack::sbuffer sbuffer;
    msgpack::pack(sbuffer, order);
    ASSERT_EQ(client.SendData(sbuffer.data(), sbuffer.size()), IPCLIB_SUCCEED);

    // receives amount of tests
    ASSERT_DURATION_LE(1, client.AwaitData(buffer, TEST_BUFFER_SIZE));
    msgpack::unpacked msg;
    msgpack::unpack(msg, buffer, TEST_BUFFER_SIZE);
    std::vector<std::string> amountOfTests;
    msg.get().convert(amountOfTests);
    ASSERT_TRUE(amountOfTests.size() == 1);
    ASSERT_TRUE(stoi(amountOfTests[0]) == 2);  // 2 tests

    ASSERT_EQ(client.SendData("OK", 2), IPCLIB_SUCCEED);

    // test 1
    ASSERT_DURATION_LE(1, client.AwaitData(buffer, TEST_BUFFER_SIZE));
    msgpack::unpacked msg2;
    msgpack::unpack(msg2, buffer, TEST_BUFFER_SIZE);
    std::vector<std::string> driveSituation;
    msg2.get().convert(driveSituation);

    Random random;
    BlackBoxDataMock mock = CreateRandomBlackBoxDataMock(random);
    TestDriveSituation(driveSituation, mock);

    // send back result of test 1
    std::vector<std::string> action{
        std::to_string(STEER_VALUE),
        std::to_string(BRAKE_VALUE)};
    sbuffer.clear();
    msgpack::pack(sbuffer, action);
    ASSERT_EQ(client.SendData(sbuffer.data(), sbuffer.size()), IPCLIB_SUCCEED);

    // test 2
    ASSERT_DURATION_LE(1, client.AwaitData(buffer, TEST_BUFFER_SIZE));
    msgpack::unpacked msg3;
    msgpack::unpack(msg3, buffer, TEST_BUFFER_SIZE);
    std::vector<std::string> driveSituation2;
    msg3.get().convert(driveSituation2);

    // test if the drivesituation is expected
    BlackBoxDataMock exampleSituation = GetExampleBlackBoxDataMock();
    TestDriveSituation(driveSituation2, exampleSituation);

    // send back result of test 2
    ASSERT_EQ(client.SendData(sbuffer.data(), sbuffer.size()), IPCLIB_SUCCEED);

    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    // initial situation
    ASSERT_DURATION_LE(1, client.AwaitData(buffer, TEST_BUFFER_SIZE));
    msgpack::unpacked msg4;
    msgpack::unpack(msg4, buffer, TEST_BUFFER_SIZE);
    std::vector<std::string> driveSituation3;
    msg4.get().convert(driveSituation3);

    // tests if the drive situation is expected
    TestDriveSituation(driveSituation3, mock);

    // send back result of initial drive situation
    ASSERT_EQ(client.SendData(sbuffer.data(), sbuffer.size()), IPCLIB_SUCCEED);

    // normal
    if (p_async)
    {
        ASSERT_DURATION_LE(1, client.AwaitData(buffer, TEST_BUFFER_SIZE));  // removes 1 data step

        msgpack::unpacked msg5;
        msgpack::unpack(msg5, buffer, TEST_BUFFER_SIZE);
        std::vector<std::string> driveSituation4;
        msg5.get().convert(driveSituation4);

        // tests if the drive situation is expected
        TestDriveSituation(driveSituation4, exampleSituation);

        // wait to make sure the blackbox can get to a finishing state
        std::this_thread::sleep_for(std::chrono::seconds(2));

        ASSERT_EQ(client.SendData(sbuffer.data(), sbuffer.size()), IPCLIB_SUCCEED);
    }

    // gets a stop command
    ASSERT_DURATION_LE(1, client.AwaitData(buffer, TEST_BUFFER_SIZE));
    ASSERT_TRUE(buffer[0] == 'S' && buffer[1] == 'T' && buffer[2] == 'O' && buffer[3] == 'P');

    // return to break connection
    ASSERT_EQ(client.SendData("OK", 2), IPCLIB_SUCCEED);
    client.Disconnect();
}

TEST_CASE(SocketBlackBoxTests, SocketTestAsync, SocketTest, (&BlackBoxSideAsync, true))
TEST_CASE(SocketBlackBoxTests, SocketTestSync, SocketTest, (&BlackBoxSideSync, false))

/// @brief this black box side should fail
void FailingBlackBox()
{
    ASSERT_THROW(BlackBoxSideAsync(), std::exception);
}

/// @brief Tests what happens when no order is sent
TEST(SocketBlackBoxTests, NoOrderSend)
{
    std::thread t = std::thread(FailingBlackBox);
    t.detach();
    std::this_thread::sleep_for(std::chrono::seconds(2));
    ClientSocket client;
    ASSERT_DURATION_LE(
        2, while (client.Initialize() != IPCLIB_SUCCEED) { std::this_thread::yield(); })
    ASSERT_EQ(client.SendData("AI ACTIVE", 9), IPCLIB_SUCCEED);
    char buffer[TEST_BUFFER_SIZE];
    ASSERT_EQ(client.AwaitData(buffer, TEST_BUFFER_SIZE), IPCLIB_SUCCEED);
    ASSERT_TRUE(buffer[0] == 'O' && buffer[1] == 'K');

    std::vector<std::string> order = {
        "Speed",
        "TopSpeed",
        "Gear",
        "Headlights",
        "SteerCmd",
        "AccelCmd",
        "BrakeCmd",
        "ClutchCmd",
        "Offroad",
        "ToMiddle",
        "ToLeft",
        "ToRight",
        "ToStart",
        "TimeOfDay",
        "Clouds",
        "Rain",
        "ACTIONORDER",
        "Steer",
        "Brake"};

    // sends required and sending data of client
    msgpack::sbuffer sbuffer;
    msgpack::pack(sbuffer, order);
    client.SendData(sbuffer.data(), static_cast<int>(sbuffer.size()));
}

/// @brief Tests what happens when no action order is sent
TEST(SocketBlackBoxTests, NoActionOrderSend)
{
    std::thread t = std::thread(FailingBlackBox);
    t.detach();
    std::this_thread::sleep_for(std::chrono::seconds(2));
    ClientSocket client;
    ASSERT_DURATION_LE(
        2, while (client.Initialize() != IPCLIB_SUCCEED) { std::this_thread::yield(); })
    ASSERT_EQ(client.SendData("AI ACTIVE", 9), IPCLIB_SUCCEED);
    char buffer[TEST_BUFFER_SIZE];
    ASSERT_EQ(client.AwaitData(buffer, TEST_BUFFER_SIZE), IPCLIB_SUCCEED);
    ASSERT_TRUE(buffer[0] == 'O' && buffer[1] == 'K');

    std::vector<std::string> order = {
        "DATAORDER",
        "Speed",
        "TopSpeed",
        "Gear",
        "Headlights",
        "SteerCmd",
        "AccelCmd",
        "BrakeCmd",
        "ClutchCmd",
        "Offroad",
        "ToMiddle",
        "ToLeft",
        "ToRight",
        "ToStart",
        "TimeOfDay",
        "Clouds",
        "Rain",
        "Steer",
        "Brake"};

    // sends required and sending data of client
    msgpack::sbuffer sbuffer;
    msgpack::pack(sbuffer, order);

    std::cout << "sending data" << std::endl;
    ASSERT_EQ(client.SendData(sbuffer.data(), static_cast<int>(sbuffer.size())), IPCLIB_SUCCEED);
    client.Disconnect();
}