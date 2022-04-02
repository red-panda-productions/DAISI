#pragma once
#include <vector>
#include <string>
#include <unordered_map>
#include "msgpack.hpp"
#include "DecisionTuple.h"
#include "ServerSocket.h"
#include "car.h"
#include "raceman.h"
#include "BlackBoxData.h"

#define SBB_BUFFER_SIZE 512

/// @brief                A class that makes a socket connection to a black box algorithm
/// @tparam BlackBoxData  The BlackBoxData type
template <class BlackBoxData>
class SocketBlackBox
{
public:
    SocketBlackBox(PCWSTR p_ip = L"127.0.0.1", int p_port = 8888);

    void Initialize();

    void Initialize(BlackBoxData& p_initialBlackBoxData, BlackBoxData* p_tests = nullptr, int p_amountOfTests = 0);

    void Shutdown();

    void SerializeBlackBoxData(msgpack::sbuffer& p_sbuffer, BlackBoxData* p_BlackBoxData);

	void DeserializeBlackBoxResults(const char* p_dataReceived, unsigned int p_size, DecisionTuple& p_decisionTuple);

    bool GetDecisions(tCarElt* p_car, tSituation* p_situation, int p_tickCount, DecisionTuple& p_decisions);

    std::vector<std::string> VariablesToReceive;

private:

    // map with function that create correct concrete decision
    using decisionConvertFunction = void (*) (std::string&, DecisionTuple&);
    std::unordered_map<std::string, decisionConvertFunction> m_variableDecisionMap;

    ServerSocket m_server;
    char m_buffer[SBB_BUFFER_SIZE];

    BlackBoxData* m_currentData = nullptr;
};

/// @brief The standard SocketBlackBox type
#define SSocketBlackBox SocketBlackBox<BlackBoxData>