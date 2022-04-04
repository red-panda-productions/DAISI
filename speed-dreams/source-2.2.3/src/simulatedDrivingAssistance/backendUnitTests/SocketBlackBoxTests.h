#pragma once
#include <gtest/gtest.h>
#include <limits>
#include <thread>
#include "ClientSocket.h"
#include "mocks/BlackBoxDataMock.h"
#define TEST_BUFFER_SIZE 512
#define TOLERANCE 0.1f
#define STEER_VALUE 1.0f
#define BRAKE_VALUE 2.0f

/// @brief				Sets up the connection between the AI and the test
/// @param  method_name The method that needs to be tested
#define SETUP(method_name) \
	std::thread t = std::thread(method_name); \
	t.detach();\
	std::this_thread::sleep_for(std::chrono::milliseconds(100));\
	ClientSocket client;\
	client.Initialize();\
	client.SendData("AI ACTIVE", 9);\
	char buffer[TEST_BUFFER_SIZE];\
	client.AwaitData(buffer, TEST_BUFFER_SIZE);\
	ASSERT_TRUE(buffer[0] == 'O' && buffer[1] == 'K');

/// @brief The black box side of the test, as these tests have to run in parallel
void BlackBoxSide()
{
	SocketBlackBox<BlackBoxDataMock> bb;
	Random random;
	BlackBoxDataMock mock = CreateRandomBlackBoxDataMock(random);
	BlackBoxDataMock exampleSituation = GetExampleBlackBoxDataMock();
	BlackBoxDataMock situations[2]{ mock,exampleSituation };

	/// intializes the black box with 2 tests
	bb.Initialize(mock, situations, 2);
	DecisionTuple decisions;

	tCarElt car;
	tSituation situation;
	// no decision should be made yet
	ASSERT_FALSE(bb.GetDecisions(&car,&situation,0, decisions));
	std::this_thread::sleep_for(std::chrono::milliseconds(30));

	// awaited the client so a decision should be here
	ASSERT_TRUE(bb.GetDecisions(&car, &situation, 0, decisions));

	// check the result
	ASSERT_ALMOST_EQ(decisions.GetSteer(), STEER_VALUE, TOLERANCE);
	ASSERT_ALMOST_EQ(decisions.GetBrake(), BRAKE_VALUE, TOLERANCE);

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
TEST(SocketBlackBoxTests, SocketTest)
{
	// creates a connection between the black box and a client
	SETUP(BlackBoxSide)

		std::vector<std::string> order = {
			"ACTIONORDER",
			"Steer",
			"Brake"
	};

	// sends required and sending data of client
	msgpack::sbuffer sbuffer;
	msgpack::pack(sbuffer, order);
	ASSERT_EQ(client.SendData(sbuffer.data(), sbuffer.size()),IPCLIB_SUCCEED);

	// receives amount of tests
	client.AwaitData(buffer, TEST_BUFFER_SIZE);
	msgpack::unpacked msg;
	msgpack::unpack(msg, buffer, TEST_BUFFER_SIZE);
	std::vector<std::string> amountOfTests;
	msg->convert(amountOfTests);
	ASSERT_TRUE(amountOfTests.size() == 1);
	ASSERT_TRUE(stoi(amountOfTests[0]) == 2); // 2 tests



	// test 1
	client.AwaitData(buffer, TEST_BUFFER_SIZE);
	msgpack::unpacked msg2;
	msgpack::unpack(msg2, buffer, TEST_BUFFER_SIZE);
	std::vector<std::string> driveSituation;
	msg2->convert(driveSituation);

	Random random;
	BlackBoxDataMock mock = CreateRandomBlackBoxDataMock(random);
	TestDriveSituation(driveSituation, mock);

	// send back result of test 1
	std::vector<std::string> action{
		std::to_string(STEER_VALUE),
		std::to_string(BRAKE_VALUE)
	};
	sbuffer.clear();
	msgpack::pack(sbuffer, action);
	ASSERT_EQ(client.SendData(sbuffer.data(), sbuffer.size()),IPCLIB_SUCCEED);

	// test 2
	client.AwaitData(buffer, TEST_BUFFER_SIZE);
	msgpack::unpacked msg3;
	msgpack::unpack(msg3, buffer, TEST_BUFFER_SIZE);
	std::vector<std::string> driveSituation2;
	msg3->convert(driveSituation2);

	// test if the drivesituation is expected
	BlackBoxDataMock exampleSituation = GetExampleBlackBoxDataMock();
	TestDriveSituation(driveSituation2, exampleSituation);

	// send back result of test 2
	ASSERT_EQ(client.SendData(sbuffer.data(), sbuffer.size()), IPCLIB_SUCCEED);

	std::this_thread::sleep_for(std::chrono::milliseconds(10));

	// initial situation
	client.AwaitData(buffer, TEST_BUFFER_SIZE);
	msgpack::unpacked msg4;
	msgpack::unpack(msg4, buffer, TEST_BUFFER_SIZE);
	std::vector<std::string> driveSituation3;
	msg4->convert(driveSituation3);

	// tests if the drive situation is expected
	TestDriveSituation(driveSituation3, mock);

	// send back result of initial drive situation
	ASSERT_EQ(client.SendData(sbuffer.data(), sbuffer.size()), IPCLIB_SUCCEED);


	// normal
	client.AwaitData(buffer, TEST_BUFFER_SIZE); //removes 1 data step
	msgpack::unpacked msg5;
	msgpack::unpack(msg5, buffer, TEST_BUFFER_SIZE);
	std::vector<std::string> driveSituation4;
	msg5->convert(driveSituation4);

	// tests if the drive situation is expected
	TestDriveSituation(driveSituation4, exampleSituation);
	ASSERT_EQ(client.SendData(sbuffer.data(), sbuffer.size()), IPCLIB_SUCCEED);

	// gets a stop command
	client.AwaitData(buffer, TEST_BUFFER_SIZE);
	ASSERT_TRUE(buffer[0] == 'S' && buffer[1] == 'T' && buffer[2] == 'O' && buffer[3] == 'P');

	// return to break connection
	ASSERT_EQ(client.SendData("OK", 2), IPCLIB_SUCCEED);
	client.Disconnect();
}

/// @brief this black box side should fail
void FailingBlackBox()
{
	ASSERT_THROW(BlackBoxSide(), std::exception);
}

/// @brief Tests what happens when no order is sent
TEST(SocketBlackBoxTests, NoOrderSend)
{
	SETUP(FailingBlackBox)

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
			"Brake"
	};

	// sends required and sending data of client
	msgpack::sbuffer sbuffer;
	msgpack::pack(sbuffer, order);
	client.SendData(sbuffer.data(), sbuffer.size());
}

/// @brief Tests what happens when no action order is sent
TEST(SocketBlackBoxTests, NoActionOrderSend)
{
	SETUP(FailingBlackBox)

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
			"Brake"
	};

	// sends required and sending data of client
	msgpack::sbuffer sbuffer;
	msgpack::pack(sbuffer, order);
	client.SendData(sbuffer.data(), sbuffer.size());
}