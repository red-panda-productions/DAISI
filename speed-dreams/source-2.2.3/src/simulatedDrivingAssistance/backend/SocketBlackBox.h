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
#include "IPCPointerManager.h"
#include "ipclib_portability.h"

#define SBB_BUFFER_SIZE 512

/// @brief                 A class that makes a socket connection to a black box algorithm
/// @tparam BlackBoxData   The BlackBoxData type
/// @tparam PointerManager A manager that manages where the data should be stored
template <class BlackBoxData, class PointerManager>
class SocketBlackBox
{
public:
    SocketBlackBox(IPC_IP_TYPE p_ip = LOCAL_HOST, int p_port = 8888);

    void Initialize();

    void Initialize(bool p_connectAsync, BlackBoxData& p_initialBlackBoxData, BlackBoxData* p_tests = nullptr, int p_amountOfTests = 0, bool* p_terminate = nullptr);

    void Shutdown();

    void SerializeBlackBoxData(msgpack::sbuffer& p_sbuffer, BlackBoxData* p_blackBoxData);

    void DeserializeBlackBoxResults(const char* p_dataReceived, unsigned int p_size, DecisionTuple& p_decisionTuple);

    bool GetDecisions(tCarElt* p_car, tSituation* p_situation, unsigned long p_tickCount, DecisionTuple& p_decisions);

    std::vector<std::string> VariablesToReceive;

private:
    // map with function that create correct concrete decision
    using decisionConvertFunction = void (*)(std::string&, DecisionTuple&);
    std::unordered_map<std::string, decisionConvertFunction> m_variableDecisionMap;

    ServerSocket m_server;
    char m_buffer[SBB_BUFFER_SIZE];

    BlackBoxData* m_currentData = nullptr;

    PointerManager m_pointerManager;

    bool m_asyncConnection = false;
};

/// @brief The standard SocketBlackBox type
#define SSocketBlackBox SocketBlackBox<BlackBoxData, SIPCPointerManager>