#pragma once
#include <gtest/gtest.h>
#include "SocketBlackBox.h"
#include "mocks/DriveSituationMock.h"
#include <limits>
#include "../rppUtils/Random.hpp"
#include <thread>
#include "ClientSocket.h"

#define TEST_BUFFER_SIZE 512

DriveSituationMock GetExampleDriveSituation()
{
	TrackPositionMock trackpos(true, 0.1, 0.2, 0.3, 0.4);
	CarInfoMock carinfo(1.1, 1.2, 1.3, 1.4, trackpos);
	PlayerInfoMock playerinfo(2.1, 2.2, 2.3, 2.4);
	EnvironmentInfoMock environmentinfo(3, 4, 5);
	return { playerinfo, carinfo, environmentinfo };
}

void BlackBoxSide()
{
	SocketBlackBox<DriveSituationMock> bb;
	CREATE_DRIVE_SITUATION_MOCK;
	DriveSituationMock exampleSituation = GetExampleDriveSituation();
	DriveSituationMock situations[2] {mock,exampleSituation};
	bb.Initialize(mock,situations,2);
	DecisionTuple decisions;
	ASSERT_FALSE(bb.GetDecisions(mock, decisions));
	std::this_thread::sleep_for(std::chrono::milliseconds(30));
	ASSERT_TRUE(bb.GetDecisions(exampleSituation, decisions));
	ASSERT_ALMOST_EQ(decisions.m_steerDecision.m_steerAmount,1.0f,0.1f);
	ASSERT_ALMOST_EQ(decisions.m_brakeDecision.m_brakeAmount,2.0f,0.1f);
	bb.Shutdown();
}

void TestDriveSituation(std::vector<std::string>& p_driveSituation,DriveSituationMock p_target)
{
	
}

TEST(SocketBlackBoxTests,SocketTest)
{
	std::thread t = std::thread(BlackBoxSide);
	t.detach();
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	ClientSocket client;
	client.SendData("AI ACTIVE", 9);

	char buffer[TEST_BUFFER_SIZE];

	client.AwaitData(buffer, TEST_BUFFER_SIZE);
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
		"ACTIONORDER",
		"Steer",
		"Brake"
	};

	msgpack::sbuffer sbuffer;
	msgpack::pack(sbuffer, order);
	client.SendData(sbuffer.data(), sbuffer.size());
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
	msg->convert(driveSituation);

	CREATE_DRIVE_SITUATION_MOCK;
	TestDriveSituation(driveSituation,mock);

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
	msg->convert(driveSituation2);

	DriveSituationMock exampleSituation = GetExampleDriveSituation();
	TestDriveSituation(driveSituation2, exampleSituation);

	client.SendData(sbuffer.data(), sbuffer.size());

	std::this_thread::sleep_for(std::chrono::milliseconds(10));
	// normal

	client.AwaitData(buffer, TEST_BUFFER_SIZE);
	msgpack::unpacked msg4;
	msgpack::unpack(msg4, buffer, TEST_BUFFER_SIZE);
	std::vector<std::string> driveSituation3;
	msg->convert(driveSituation3);

	TestDriveSituation(driveSituation3, exampleSituation);
	client.SendData(sbuffer.data(), sbuffer.size());

	client.AwaitData(buffer, TEST_BUFFER_SIZE); //removes 1 data step

	client.AwaitData(buffer, TEST_BUFFER_SIZE);
	ASSERT_TRUE(buffer[0] == 'S' && buffer[1] == 'T' && buffer[2] == 'O' && buffer[3] == 'P');

	client.Disconnect();
}