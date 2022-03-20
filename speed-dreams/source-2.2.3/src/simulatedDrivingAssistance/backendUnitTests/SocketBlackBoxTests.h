#pragma once
#include <gtest/gtest.h>
#include "mocks/DriveSituationMock.h"
#include <limits>
#include <thread>
#include "ClientSocket.h"
#define TEST_BUFFER_SIZE 512

#define SETUP(method_name) \
	std::thread t = std::thread(method_name); \
	t.detach();\
	std::this_thread::sleep_for(std::chrono::milliseconds(100));\
	ClientSocket client;\
	client.SendData("AI ACTIVE", 9);\
	char buffer[TEST_BUFFER_SIZE];\
	client.AwaitData(buffer, TEST_BUFFER_SIZE);\
	ASSERT_TRUE(buffer[0] == 'O' && buffer[1] == 'K');




/// @brief The black box side of the test, as these tests have to run in parallel
void BlackBoxSide()
{
	SocketBlackBox<DriveSituationMock> bb;
	CREATE_DRIVE_SITUATION_MOCK;
	DriveSituationMock exampleSituation = GetExampleDriveSituation();
	DriveSituationMock situations[2] {mock,exampleSituation};

	/// intializes the black box with 2 tests
	bb.Initialize(mock,situations,2);
	DecisionTuple decisions;

	// no decision should be made yet
	ASSERT_FALSE(bb.GetDecisions(mock, decisions));
	std::this_thread::sleep_for(std::chrono::milliseconds(30));

	// awaited the client so a decision should be here
	ASSERT_TRUE(bb.GetDecisions(exampleSituation, decisions));

	// check the result
	ASSERT_ALMOST_EQ(decisions.GetSteer(), 1.0f, 0.1f);
	ASSERT_ALMOST_EQ(decisions.GetBrake(), 2.0f, 0.1f);

	// shut the server down
	bb.Shutdown();
}

/// @brief					 Tests if the parsed drivesituation is the same as the target
/// @param  p_driveSituation Parsed drive situation
/// @param  p_target		 The target
void TestDriveSituation(std::vector<std::string>& p_driveSituation,DriveSituationMock p_target)
{
	ASSERT_ALMOST_EQ(stof(p_driveSituation[0]), p_target.GetCarInfo()->Speed(),0.01);
	ASSERT_ALMOST_EQ(stof(p_driveSituation[1]), p_target.GetCarInfo()->TopSpeed(),0.01);
	ASSERT_EQ(stoi(p_driveSituation[2]), p_target.GetCarInfo()->Gear());
	ASSERT_EQ(stoi(p_driveSituation[3]), p_target.GetCarInfo()->Headlights() ? 1 : 0);
	ASSERT_ALMOST_EQ(stof(p_driveSituation[4]), p_target.GetPlayerInfo()->SteerCmd(),0.01);
	ASSERT_ALMOST_EQ(stof(p_driveSituation[5]), p_target.GetPlayerInfo()->AccelCmd(),0.01);
	ASSERT_ALMOST_EQ(stof(p_driveSituation[6]), p_target.GetPlayerInfo()->BrakeCmd(),0.01);
	ASSERT_ALMOST_EQ(stof(p_driveSituation[7]), p_target.GetPlayerInfo()->ClutchCmd(),0.01);
	ASSERT_EQ(stoi(p_driveSituation[8]), p_target.GetCarInfo()->trackPosition.Offroad() ? 1 : 0);
	ASSERT_ALMOST_EQ(stof(p_driveSituation[9]), p_target.GetCarInfo()->trackPosition.ToMiddle(),0.01);
	ASSERT_ALMOST_EQ(stof(p_driveSituation[10]), p_target.GetCarInfo()->trackPosition.ToLeft(),0.01);
	ASSERT_ALMOST_EQ(stof(p_driveSituation[11]), p_target.GetCarInfo()->trackPosition.ToRight(),0.01);
	ASSERT_ALMOST_EQ(stof(p_driveSituation[12]), p_target.GetCarInfo()->trackPosition.ToStart(),0.01);
	ASSERT_ALMOST_EQ(stof(p_driveSituation[13]), p_target.GetEnvironmentInfo()->TimeOfDay(),0.01);
	ASSERT_ALMOST_EQ(stof(p_driveSituation[14]), p_target.GetEnvironmentInfo()->Clouds(),0.01);
	ASSERT_ALMOST_EQ(stof(p_driveSituation[15]), p_target.GetEnvironmentInfo()->Rain(),0.01);
}

/// @brief Tests an entire run of the framework
TEST(SocketBlackBoxTests,SocketTest)
{
	// creates a connection between the black box and a client
	SETUP(BlackBoxSide)

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
		"ACTIONORDER",
		"Steer",
		"Brake"
	};

	// sends required and sending data of client
	msgpack::sbuffer sbuffer;
	msgpack::pack(sbuffer, order);
	client.SendData(sbuffer.data(), sbuffer.size());

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

	CREATE_DRIVE_SITUATION_MOCK;
	TestDriveSituation(driveSituation,mock);

	// send back result of test 1
	std::vector<std::string> action{
		"1.0000",
		"2.0000"
	};
	sbuffer.clear();
	msgpack::pack(sbuffer, action);
	client.SendData(sbuffer.data(), sbuffer.size());

	// test 2
	client.AwaitData(buffer, TEST_BUFFER_SIZE);
	msgpack::unpacked msg3;
	msgpack::unpack(msg3, buffer, TEST_BUFFER_SIZE);
	std::vector<std::string> driveSituation2;
	msg3->convert(driveSituation2);

	// test if the drivesituation is expected
	DriveSituationMock exampleSituation = GetExampleDriveSituation();
	TestDriveSituation(driveSituation2, exampleSituation);

	// send back result of test 2
	client.SendData(sbuffer.data(), sbuffer.size());

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
	client.SendData(sbuffer.data(), sbuffer.size());


	// normal
	client.AwaitData(buffer, TEST_BUFFER_SIZE); //removes 1 data step
	msgpack::unpacked msg5;
	msgpack::unpack(msg5, buffer, TEST_BUFFER_SIZE);
	std::vector<std::string> driveSituation4;
	msg5->convert(driveSituation4);

	// tests if the drive situation is expected
	TestDriveSituation(driveSituation4, exampleSituation);
	client.SendData(sbuffer.data(), sbuffer.size());

	// gets a stop command
	client.AwaitData(buffer, TEST_BUFFER_SIZE);
	ASSERT_TRUE(buffer[0] == 'S' && buffer[1] == 'T' && buffer[2] == 'O' && buffer[3] == 'P');

	// return to break connection
	client.SendData("OK", 2);
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