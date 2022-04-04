#include <gtest/gtest.h>
#include "TestUtils.h"
#include "../rppUtils/Random.hpp"
#include "msgpack.hpp"
#include "MsgpackSerializeTests.h"
#include "SocketBlackBoxTests.h"
#include "DecisionTuple.h"
#include "mocks/BlackBoxDataMock.h"

#define STEER "Steer"
#define BRAKE "Brake"
#define NON_EXISTING_DECISION_KEY "NON_EXISTING_DECISION_KEY"

#define TOLERANCE 0.0001

/// @brief Tests if all (currently) existing variables can be deserialized into a decision correctly
TEST(MsgpackDeserializeTests, Deserialize)
{
    Random random;
    float controlSteerValue = random.NextFloat(1000);
    float controlBrakeValue = random.NextFloat(1000);

    std::vector<std::string> mockMessageFromBlackBox = { std::to_string(controlSteerValue), std::to_string(controlBrakeValue) };
    msgpack::sbuffer sbuffer;
    msgpack::pack(sbuffer, mockMessageFromBlackBox);

    SocketBlackBox<BlackBoxDataMock,PointerManagerMock> socketBlackBox;
    socketBlackBox.Initialize();

    socketBlackBox.VariablesToReceive = { STEER, BRAKE };

    DecisionTuple decisionTuple;
    socketBlackBox.DeserializeBlackBoxResults(sbuffer.data(), sbuffer.size(), decisionTuple);

    ASSERT_ALMOST_EQ(decisionTuple.GetSteer(), controlSteerValue, TOLERANCE);
    ASSERT_ALMOST_EQ(decisionTuple.GetBrake(), controlBrakeValue, TOLERANCE);
}

/// @brief Tests if the program throws when there are no variables to receive
TEST(MsgpackDeserializeTests, NoVariablesToReceive)
{
    Random random;
    float controlSteerValue = random.NextFloat(1000);
    float controlBrakeValue = random.NextFloat(1000);

    std::vector<std::string> mockMessageFromBlackBox = { std::to_string(controlSteerValue), std::to_string(controlBrakeValue) };
    msgpack::sbuffer sbuffer;
    msgpack::pack(sbuffer, mockMessageFromBlackBox);

    SocketBlackBox<BlackBoxDataMock,PointerManagerMock> socketBlackBox;
    socketBlackBox.Initialize();

    DecisionTuple decisionTuple;
    ASSERT_THROW(socketBlackBox.DeserializeBlackBoxResults(sbuffer.data(), sbuffer.size(), decisionTuple), std::exception);
}

/// @brief Tests if a variable is Not A Number when received data is not parsable to the correct type.
TEST(MsgpackDeserializeTests, UnparsableData)
{
    Random random;
    std::string unparsableString = "This is not a float";
    float controlBrakeValue = random.NextFloat();

    std::vector<std::string> mockMessageFromBlackBox = { unparsableString, std::to_string(controlBrakeValue) };
    msgpack::sbuffer sbuffer;
    msgpack::pack(sbuffer, mockMessageFromBlackBox);

    SocketBlackBox<BlackBoxDataMock,PointerManagerMock> socketBlackBox;
    socketBlackBox.Initialize();

    socketBlackBox.VariablesToReceive = { STEER, BRAKE };

    DecisionTuple decisionTuple;
    socketBlackBox.DeserializeBlackBoxResults(sbuffer.data(), sbuffer.size(), decisionTuple);

    ASSERT_TRUE(isnan(decisionTuple.GetSteer()));
    ASSERT_ALMOST_EQ(decisionTuple.GetBrake(), controlBrakeValue, 0.000001);
}

/// @brief Tests if program throws when if a variable to parse does not exist in the function map.
TEST(MsgpackDeserializeTests, NonExistingDecisionKey)
{
    Random random;
    std::string unparsableString = "This is not a float";
    float controlBrakeValue = random.NextFloat();

    std::vector<std::string> mockMessageFromBlackBox = { unparsableString, std::to_string(controlBrakeValue) };
    msgpack::sbuffer sbuffer;
    msgpack::pack(sbuffer, mockMessageFromBlackBox);

    SocketBlackBox<BlackBoxDataMock,PointerManagerMock> socketBlackBox;
    socketBlackBox.Initialize();

    socketBlackBox.VariablesToReceive = { NON_EXISTING_DECISION_KEY, BRAKE };

    DecisionTuple decisionTuple;
    ASSERT_THROW(socketBlackBox.DeserializeBlackBoxResults(sbuffer.data(), sbuffer.size(), decisionTuple), std::exception);
}

/// @brief Tests if the program throws when more data has been send than expected.
TEST(MsgpackDeserializeTests, TooManyVariablesReceived)
{
    Random random;
    float controlSteerValue = random.NextFloat();
    float controlBrakeValue = random.NextFloat();
    float extraValue = random.NextFloat();

    std::vector<std::string> mockMessageFromBlackBox = { std::to_string(controlSteerValue), std::to_string(controlBrakeValue), std::to_string(extraValue) };
    msgpack::sbuffer sbuffer;
    msgpack::pack(sbuffer, mockMessageFromBlackBox);

    SocketBlackBox<BlackBoxDataMock,PointerManagerMock> socketBlackBox;
    socketBlackBox.Initialize();

    socketBlackBox.VariablesToReceive = { STEER, BRAKE };

    DecisionTuple decisionTuple;
    ASSERT_THROW(socketBlackBox.DeserializeBlackBoxResults(sbuffer.data(), sbuffer.size(), decisionTuple), std::exception);
}

/// @brief Tests if the program throws when less data has been send than expected.
TEST(MsgpackDeserializeTests, TooLittleVariablesReceived)
{
    Random random;
    float controlSteerValue = random.NextFloat();

    std::vector<std::string> mockMessageFromBlackBox = { std::to_string(controlSteerValue) };
    msgpack::sbuffer sbuffer;
    msgpack::pack(sbuffer, mockMessageFromBlackBox);

    SocketBlackBox<BlackBoxDataMock,PointerManagerMock> socketBlackBox;
    socketBlackBox.Initialize();

    socketBlackBox.VariablesToReceive = { STEER, BRAKE };

    DecisionTuple decisionTuple;
    ASSERT_THROW(socketBlackBox.DeserializeBlackBoxResults(sbuffer.data(), sbuffer.size(), decisionTuple), std::exception);
}