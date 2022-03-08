#include <gtest/gtest.h>
#include "Utils.h"
#include "msgpack.hpp"
#include "SocketBlackBox.h"
#include "MsgpackSerializeTests.h"
#include "mocks/DriveSituationMock.h"
#include "DecisionTuple.h"

TEST(MsgpackDeserializeTests, Deserialize)
{
    float controlSteerValue = randomFloat(1000);
    float controlBrakeValue = randomFloat(1000);

    std::vector<std::string> mockMessageFromBlackBox = {std::to_string(controlSteerValue), std::to_string(controlBrakeValue)};
    msgpack::sbuffer sbuffer;
    msgpack::pack(sbuffer, mockMessageFromBlackBox);

    SocketBlackBox<DriveSituationMock> socketBlackBox;
    socketBlackBox.Initialize();

    socketBlackBox.m_variablesToReceive = {"Steer", "Brake"};

    DecisionTuple decisionTuple;
    socketBlackBox.DeserializeBlackBoxResults(decisionTuple, sbuffer.data(), sbuffer.size());

    ASSERT_ALMOST_EQ(decisionTuple.steerDecision.m_steerAmount, controlSteerValue, 0.000001);
    ASSERT_ALMOST_EQ(decisionTuple.brakeDecision.m_brakeAmount, controlBrakeValue, 0.000001);
}

TEST(MsgpackDeserializeTests, NoVariablesToReceive)
{
    float controlSteerValue = randomFloat();
    float controlBrakeValue = randomFloat();

    std::vector<std::string> mockMessageFromBlackBox = {std::to_string(controlSteerValue), std::to_string(controlBrakeValue)};
    msgpack::sbuffer sbuffer;
    msgpack::pack(sbuffer, mockMessageFromBlackBox);

    SocketBlackBox<DriveSituationMock> socketBlackBox;
    socketBlackBox.Initialize();

    DecisionTuple decisionTuple;
    ASSERT_THROW(socketBlackBox.DeserializeBlackBoxResults(decisionTuple, sbuffer.data(), sbuffer.size()), std::exception);
}

TEST(MsgpackDeserializeTests, UnparsableData)
{
    std::string unparsableString = "This is not a float";
    float controlBrakeValue = randomFloat();

    std::vector<std::string> mockMessageFromBlackBox = {unparsableString, std::to_string(controlBrakeValue)};
    msgpack::sbuffer sbuffer;
    msgpack::pack(sbuffer, mockMessageFromBlackBox);

    SocketBlackBox<DriveSituationMock> socketBlackBox;
    socketBlackBox.Initialize();

    socketBlackBox.m_variablesToReceive = {"Steer", "Brake"};

    DecisionTuple decisionTuple;
    socketBlackBox.DeserializeBlackBoxResults(decisionTuple, sbuffer.data(), sbuffer.size());

    ASSERT_TRUE(isnan(decisionTuple.steerDecision.m_steerAmount));
    ASSERT_ALMOST_EQ(decisionTuple.brakeDecision.m_brakeAmount, controlBrakeValue, 0.000001);
}

TEST(MsgpackDeserializeTests, NonExistingDecisionKey)
{
    std::string unparsableString = "This is not a float";
    float controlBrakeValue = randomFloat();

    std::vector<std::string> mockMessageFromBlackBox = {unparsableString, std::to_string(controlBrakeValue)};
    msgpack::sbuffer sbuffer;
    msgpack::pack(sbuffer, mockMessageFromBlackBox);

    SocketBlackBox<DriveSituationMock> socketBlackBox;
    socketBlackBox.Initialize();

    socketBlackBox.m_variablesToReceive = {"NON_EXISTING_DECISION_KEY", "Brake"};

    DecisionTuple decisionTuple;
    ASSERT_THROW(socketBlackBox.DeserializeBlackBoxResults(decisionTuple, sbuffer.data(), sbuffer.size()), std::exception);
}

TEST(MsgpackDeserializeTests, TooManyVariablesReceived)
{
    float controlSteerValue = randomFloat();
    float controlBrakeValue = randomFloat();
    float extraValue = randomFloat();

    std::vector<std::string> mockMessageFromBlackBox = {std::to_string(controlSteerValue), std::to_string(controlBrakeValue), std::to_string(extraValue)};
    msgpack::sbuffer sbuffer;
    msgpack::pack(sbuffer, mockMessageFromBlackBox);

    SocketBlackBox<DriveSituationMock> socketBlackBox;
    socketBlackBox.Initialize();

    socketBlackBox.m_variablesToReceive = {"Steer", "Brake"};

    DecisionTuple decisionTuple;
    ASSERT_THROW(socketBlackBox.DeserializeBlackBoxResults(decisionTuple, sbuffer.data(), sbuffer.size()), std::exception);
}

TEST(MsgpackDeserializeTests, TooLittleVariablesReceived)
{
    float controlSteerValue = randomFloat();

    std::vector<std::string> mockMessageFromBlackBox = {std::to_string(controlSteerValue)};
    msgpack::sbuffer sbuffer;
    msgpack::pack(sbuffer, mockMessageFromBlackBox);

    SocketBlackBox<DriveSituationMock> socketBlackBox;
    socketBlackBox.Initialize();

    socketBlackBox.m_variablesToReceive = {"Steer", "Brake"};

    DecisionTuple decisionTuple;
    ASSERT_THROW(socketBlackBox.DeserializeBlackBoxResults(decisionTuple, sbuffer.data(), sbuffer.size()), std::exception);
}