#pragma once
#include "SocketBlackBox.h"
#include <string>

#include "DecisionMaker.h"
#include "../rppUtils/RppUtils.hpp"

#define CREATE_SOCKET_BLACKBOX_IMPLEMENTATION(p_type1, p_type2)                                                                                                       \
    template SocketBlackBox<p_type1, p_type2>::SocketBlackBox(PCWSTR p_ip, int p_port);                                                                               \
    template void SocketBlackBox<p_type1, p_type2>::Initialize();                                                                                                     \
    template void SocketBlackBox<p_type1, p_type2>::Initialize(bool p_connectAsync, BlackBoxData& p_initialBlackBoxData, BlackBoxData* p_tests, int p_amountOfTests); \
    template void SocketBlackBox<p_type1, p_type2>::Shutdown();                                                                                                       \
    template void SocketBlackBox<p_type1, p_type2>::SerializeBlackBoxData(msgpack::sbuffer& p_sbuffer, BlackBoxData* p_blackBoxData);                                 \
    template void SocketBlackBox<p_type1, p_type2>::DeserializeBlackBoxResults(const char* p_dataReceived, unsigned int p_size, DecisionTuple& p_decisionTuple);      \
    template bool SocketBlackBox<p_type1, p_type2>::GetDecisions(tCarElt* p_car, tSituation* p_situation, unsigned long p_tickCount, DecisionTuple& p_decisions);

// inserts value from BlackBoxData variables in vector
#define PUSH_BACK_DS(p_getInfo)                 [](std::vector<std::string>& p_values, BlackBoxData& p_blackBoxData) { p_values.push_back(std::to_string(p_blackBoxData.p_GetInfo)); }
#define INSERT_CAR_INFO(p_variable)             PUSH_BACK_DS(GetCarInfo()->p_variable)
#define INSERT_ENVIRONMENT_INFO(p_variable)     PUSH_BACK_DS(GetEnvironmentInfo()->p_variable)
#define INSERT_TRACK_LOCAL_POSITION(p_variable) PUSH_BACK_DS(GetCarInfo()->TrackLocalPosition()->p_variable)
#define INSERT_PLAYER_INFO(p_variable)          PUSH_BACK_DS(GetPlayerInfo()->p_variable)

// converts abstract decision to concrete decision with correct value
#define DECISION_LAMBDA(p_function) [](std::string& p_string, DecisionTuple& p_decisionTuple) { p_function; }
#define CONVERT_TO_STEER_DECISION   DECISION_LAMBDA(p_decisionTuple.SetSteer(stringToFloat(p_string)))
#define CONVERT_TO_BRAKE_DECISION   DECISION_LAMBDA(p_decisionTuple.SetBrake(stringToFloat(p_string)))
#define CONVERT_TO_GEAR_DECISION    DECISION_LAMBDA(p_decisionTuple.SetGear(std::stoi(p_string)))
#define CONVERT_TO_ACCEL_DECISION   DECISION_LAMBDA(p_decisionTuple.SetAccel(stringToFloat(p_string)))

template <class BlackBoxData, class PointerManager>
SocketBlackBox<BlackBoxData, PointerManager>::SocketBlackBox(PCWSTR p_ip, int p_port)
    : m_server(p_ip, p_port)
{
}

/// @brief Sets keys and values for the functions that retrieve the correct information.
template <class BlackBoxData, class PointerManager>
void SocketBlackBox<BlackBoxData, PointerManager>::Initialize()
{
    m_server.Initialize();
    // Decision functions
    m_variableDecisionMap["Steer"] = CONVERT_TO_STEER_DECISION;
    m_variableDecisionMap["Brake"] = CONVERT_TO_BRAKE_DECISION;
    m_variableDecisionMap["Gear"] = CONVERT_TO_GEAR_DECISION;
    m_variableDecisionMap["Accel"] = CONVERT_TO_ACCEL_DECISION;
}

/// @brief Sets keys and values for the functions that retrieve the correct information. Also initializes the AI
/// @param p_connectAsync True if blackbox will run async (not waiting for response), false if sync (wait for response)
/// @param p_initialBlackBoxData The initial drive situation
/// @param p_tests Control data used in tests
/// @param p_amountOfTests Amount of tests in p_tests
template <class BlackBoxData, class PointerManager>
void SocketBlackBox<BlackBoxData, PointerManager>::Initialize(bool p_connectAsync, BlackBoxData& p_initialBlackBoxData, BlackBoxData* p_tests, int p_amountOfTests)
{
    Initialize();
    std::cout << "Connecting ";
    if (p_connectAsync)
    {
        m_asyncConnection = true;
        std::cout << "a";
    }
    std::cout << "sync" << std::endl;
    m_server.AwaitClientConnection();
    m_server.AwaitData(m_buffer, SBB_BUFFER_SIZE);
    if (std::string(m_buffer) != "AI ACTIVE") throw std::exception("Black Box send wrong message: AI ACTIVE expected");
    m_server.SendData("OK", 2);
    m_server.AwaitData(m_buffer, SBB_BUFFER_SIZE);
    msgpack::unpacked msg;
    msgpack::unpack(msg, m_buffer, SBB_BUFFER_SIZE);
    std::vector<std::string> orderVec;
    msg->convert(orderVec);
    int i = 0;
    if (orderVec[i] != "ACTIONORDER") throw std::exception("Black box send wrong message: ACTIONORDER expected");
    i++;
    while (i < orderVec.size())
    {
        VariablesToReceive.push_back(orderVec[i++]);
    }

    // testing can be done here later
    msgpack::sbuffer sbuffer;
    std::string data[1] = {std::to_string(p_amountOfTests)};
    msgpack::pack(sbuffer, data);
    m_server.SendData(sbuffer.data(), sbuffer.size());

    m_server.AwaitData(m_buffer, SBB_BUFFER_SIZE);
    if (m_buffer[0] != 'O' || m_buffer[1] != 'K') throw std::exception("Black box send wrong message: OK expected");

    DecisionTuple decisionTuple;
    for (int i = 0; i < p_amountOfTests; i++)
    {
        sbuffer.clear();
        SerializeBlackBoxData(sbuffer, &p_tests[i]);
        m_server.SendData(sbuffer.data(), sbuffer.size());
        m_server.AwaitData(m_buffer, SBB_BUFFER_SIZE);
        DeserializeBlackBoxResults(m_buffer, SBB_BUFFER_SIZE, decisionTuple);
    }

    sbuffer.clear();
    SerializeBlackBoxData(sbuffer, &p_initialBlackBoxData);
    m_server.SendData(sbuffer.data(), sbuffer.size());
    m_server.ReceiveDataAsync();
}

/// @brief Shuts the black box down
template <class BlackBoxData, class PointerManager>
void SocketBlackBox<BlackBoxData, PointerManager>::Shutdown()
{
    if (!m_server.Connected()) return;
    m_server.AwaitData(m_buffer, SBB_BUFFER_SIZE);
    m_server.SendData("STOP", 4);
    m_server.AwaitData(m_buffer, SBB_BUFFER_SIZE);
    if (m_buffer[0] != 'O' || m_buffer[1] != 'K') throw std::exception("Client sent wrong reply");
    m_server.CloseServer();
    VariablesToReceive.clear();
    m_variableDecisionMap.clear();
}

/// @brief                  Inserts a string of value of a pointer into a msgpack message
/// @param p_sbuffer        Buffer to pack data in
/// @param p_BlackBoxData Drive situation to serialize
template <class BlackBoxData, class PointerManager>
void SocketBlackBox<BlackBoxData, PointerManager>::SerializeBlackBoxData(msgpack::sbuffer& p_sbuffer, BlackBoxData* p_blackBoxData)
{
    std::vector<std::string> dataToSerialize;

    BlackBoxData* pointer = m_pointerManager.GetDataPointer();

    *pointer = *p_blackBoxData;

    dataToSerialize.emplace_back("1");

    msgpack::pack(p_sbuffer, dataToSerialize);
}

/// @brief                 Deserializes received data and makes a decision array from this data
/// @param p_decisionTuple Decision array to put decisions in
/// @param p_dataReceived  Data received from black box
/// @param p_size          Size of received data
template <class BlackBoxData, class PointerManager>
void SocketBlackBox<BlackBoxData, PointerManager>::DeserializeBlackBoxResults(const char* p_dataReceived, unsigned int p_size, DecisionTuple& p_decisionTuple)
{
    // unpack
    msgpack::unpacked msg;
    msgpack::unpack(msg, p_dataReceived, p_size);

    // convert
    msgpack::object obj = msg.get();
    std::vector<std::string> resultVec;
    obj.convert(resultVec);

    if (VariablesToReceive.empty()) throw std::exception("No variables to receive");
    if (VariablesToReceive.size() != resultVec.size())
        throw std::exception("Number of variables received does not match number of expected variables to receive");
    for (int i = 0; i < VariablesToReceive.size(); i++)
    {
        try
        {
            m_variableDecisionMap.at(VariablesToReceive[i])(resultVec.at(i), p_decisionTuple);
        }
        catch (std::exception& e)
        {
            throw e;
        }
    }
}

/// @brief Sends serialized drive situation to black box. Deserializes data received by black box.
///        Makes decisions from received data.
/// @param p_car       Car to base decisions off.
/// @param p_situation Situation to base decisions off.
/// @param p_tickCount The current tick count.
/// @param p_decisions Decision tuple to store decisions in
/// @return returns true if a decision is made, else false (will always return true if connection is not async)
template <class BlackBoxData, class PointerManager>
bool SocketBlackBox<BlackBoxData, PointerManager>::GetDecisions(tCarElt* p_car, tSituation* p_situation, unsigned long p_tickCount, DecisionTuple& p_decisions)
{
    if (!m_asyncConnection)
        m_server.AwaitData(m_buffer, SBB_BUFFER_SIZE);
    else if (!m_server.GetData(m_buffer, SBB_BUFFER_SIZE))
        return false;
    msgpack::sbuffer sbuffer;

    delete m_currentData;
    m_currentData = new BlackBoxData(p_car, p_situation, p_tickCount, m_pointerManager.GetSegmentPointer(), LOOKAHEAD_SEGMENTS);
    SerializeBlackBoxData(sbuffer, m_currentData);
    m_server.SendData(sbuffer.data(), sbuffer.size());
    m_server.ReceiveDataAsync();

    DeserializeBlackBoxResults(m_buffer, SBB_BUFFER_SIZE, p_decisions);

    return true;
}
