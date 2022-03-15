#pragma once
#include <vector>
#include <string>
#include <unordered_map>
#include "msgpack.hpp"
#include "DecisionTuple.h"
#include "ServerSocket.h"
#include "DriveSituation.h"

#define SBB_BUFFER_SIZE 512

/// @brief                  A class that makes a socket connection to a black box algorithm
/// @tparam DriveSituation  The DriveSituation type
template <class DriveSituation>
class SocketBlackBox
{
public:

    SocketBlackBox(PCWSTR p_ip = L"127.0.0.1", int p_port = 8888) : m_server(p_ip, p_port)
    {

    }

    void Initialize();

    void Initialize(DriveSituation& p_initialDriveSituation, DriveSituation* p_tests = nullptr, int p_amountOfTests = 1);

    void Shutdown();

    void SerializeDriveSituation(msgpack::sbuffer& p_sbuffer, DriveSituation& p_driveSituation);

	void DeserializeBlackBoxResults(const char* p_dataReceived, unsigned int p_size, DecisionTuple& p_decisions);

    bool GetDecisions(DriveSituation& p_driveSituation, DecisionTuple& p_decisions);

    std::vector<std::string> m_variablesToSend;
    std::vector<std::string> m_variablesToReceive;

    // map with functions that insert correct variable value in vector
    using inserterFunction = void (*) (std::vector<std::string>&, DriveSituation&);
    std::unordered_map<std::string, inserterFunction> m_variableConvertAndInsertMap;

    // map with function that create correct concrete decision
    using decisionConvertFunction = void (*) (std::string&, DecisionTuple&);
    std::unordered_map<std::string, decisionConvertFunction> m_variableDecisionMap;

private:

    ServerSocket m_server;
    char m_buffer[SBB_BUFFER_SIZE];
};

/// @brief The standard SocketBlackBox type
#define SSocketBlackBox SocketBlackBox<DriveSituation>