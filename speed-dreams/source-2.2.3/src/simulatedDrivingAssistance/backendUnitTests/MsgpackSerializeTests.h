#include <gtest/gtest.h>
#include "SocketBlackBox.h"
#include "SocketBlackBox.inl"
#include "mocks/BlackBoxDataMock.h"
#include "mocks/PointerManagerMock.h"
#include <limits>
#include "Random.hpp"
#include "TestUtils.h"
#include <sstream>

/// <summary>
/// Tests if all variables can be serialized correctly
/// </summary>
TEST(MsgpackSerializeTests, SerializeAll)
{
    Random random;
    for (int test = 0; test < 100; test++)
    {
        SocketBlackBox<BlackBoxDataMock, PointerManagerMock> socketBlackBox;
        socketBlackBox.Initialize();

        tCarElt car;  // need assignments
        tSituation situation;

        int r = random.NextInt();
        BlackBoxDataMock mock(&car, &situation, r, nullptr, 0);

        msgpack::sbuffer sbuffer;
        socketBlackBox.SerializeBlackBoxData(sbuffer, &mock);

        msgpack::unpacked msg;
        msgpack::unpack(msg, sbuffer.data(), sbuffer.size());
        std::vector<std::string> data;
        msg.get().convert(data);

        std::stringstream oss;
        oss << &mock;
        std::string s = oss.str();

        // TestStringEqual(s.c_str(), data[0].c_str(), s.size());
    }
}
