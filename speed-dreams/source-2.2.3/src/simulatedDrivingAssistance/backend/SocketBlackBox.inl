#pragma once
#include "SocketBlackBox.h"
#include <string>
#include "../rppUtils/RppUtils.hpp"

#define CREATE_SOCKET_BLACKBOX_IMPLEMENTATION(type) \
    template void SocketBlackBox<type>::Initialize();\
	template void SocketBlackBox<type>::Initialize(DriveSituation& p_initialDriveSituation, DriveSituation* p_tests, int p_amountOfTests);\
	template void SocketBlackBox<type>::Shutdown();\
	template void SocketBlackBox<type>::SerializeDriveSituation(msgpack::sbuffer& p_sbuffer, DriveSituation& p_driveSituation);\
    template void SocketBlackBox<type>::DeserializeBlackBoxResults(const char* p_dataReceived, unsigned int p_size, DecisionTuple& p_decisionTuple);\
    template bool SocketBlackBox<type>::GetDecisions(DriveSituation& p_driveSituation, DecisionTuple& p_decisions);

// inserts value from DriveSituation variables in vector
#define PUSH_BACK_DS(p_GetInfo) [](std::vector<std::string>& p_values, DriveSituation& p_ds){p_values.push_back(std::to_string(p_ds.p_GetInfo));}
#define INSERT_CAR_INFO(p_variable) PUSH_BACK_DS(GetCarInfo()->p_variable)
#define INSERT_ENVIRONMENT_INFO(p_variable) PUSH_BACK_DS(GetEnvironmentInfo()->p_variable)
#define INSERT_TRACK_LOCAL_POSITION(p_variable) PUSH_BACK_DS(GetCarInfo()->TrackLocalPosition()->p_variable)
#define INSERT_PLAYER_INFO(p_variable) PUSH_BACK_DS(GetPlayerInfo()->p_variable)

// converts abstract decision to concrete decision with correct value
#define DECISION_LAMBDA(p_function) [](std::string& p_string, DecisionTuple& p_decisionTuple) {p_function;}
#define CONVERT_TO_STEER_DECISION DECISION_LAMBDA(p_decisionTuple.m_steerDecision.m_steerAmount = stringToFloat(p_string))
#define CONVERT_TO_BRAKE_DECISION DECISION_LAMBDA(p_decisionTuple.m_brakeDecision.m_brakeAmount = stringToFloat(p_string))

/// @brief Sets keys and values for the functions that retrieve the correct information.
template <class DriveSituation>
void SocketBlackBox<DriveSituation>::Initialize()
{
    //CarInfo functions
    m_variableConvertAndInsertMap["Speed"] = INSERT_CAR_INFO(Speed());
    m_variableConvertAndInsertMap["TopSpeed"] = INSERT_CAR_INFO(TopSpeed());
    m_variableConvertAndInsertMap["Gear"] = INSERT_CAR_INFO(Gear());
    m_variableConvertAndInsertMap["Headlights"] = INSERT_CAR_INFO(Headlights());

    //EnvironmentInfo functions
    m_variableConvertAndInsertMap["TimeOfDay"] = INSERT_ENVIRONMENT_INFO(TimeOfDay());
    m_variableConvertAndInsertMap["Clouds"] = INSERT_ENVIRONMENT_INFO(Clouds());
    m_variableConvertAndInsertMap["Rain"] = INSERT_ENVIRONMENT_INFO(Rain());

    //TrackLocalPosition functions
    m_variableConvertAndInsertMap["Offroad"] = INSERT_TRACK_LOCAL_POSITION(Offroad());
    m_variableConvertAndInsertMap["ToMiddle"] = INSERT_TRACK_LOCAL_POSITION(ToMiddle());
    m_variableConvertAndInsertMap["ToLeft"] = INSERT_TRACK_LOCAL_POSITION(ToLeft());
    m_variableConvertAndInsertMap["ToRight"] = INSERT_TRACK_LOCAL_POSITION(ToRight());
    m_variableConvertAndInsertMap["ToStart"] = INSERT_TRACK_LOCAL_POSITION(ToStart());

    //PlayerInfo functions
    m_variableConvertAndInsertMap["AccelCmd"] = INSERT_PLAYER_INFO(AccelCmd());
    m_variableConvertAndInsertMap["BrakeCmd"] = INSERT_PLAYER_INFO(BrakeCmd());
    m_variableConvertAndInsertMap["ClutchCmd"] = INSERT_PLAYER_INFO(ClutchCmd());
    m_variableConvertAndInsertMap["SteerCmd"] = INSERT_PLAYER_INFO(SteerCmd());


    //Decision functions
    m_variableDecisionMap["Steer"] = CONVERT_TO_STEER_DECISION;
    m_variableDecisionMap["Brake"] = CONVERT_TO_BRAKE_DECISION;
}


/// @brief                          Sets keys and values for the functions that retrieve the correct information. Also initializes the AI
/// @param p_initialDriveSituation  The initial drive situation
template <class DriveSituation>
void SocketBlackBox<DriveSituation>::Initialize(DriveSituation& p_initialDriveSituation, DriveSituation* p_tests, int p_amountOfTests)
{
    Initialize();
    m_server.ConnectAsync();
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
    if (orderVec[i++] != "DATAORDER") throw std::exception("Black Box send wrong message: DATAORDER expected");
    while (i < orderVec.size() && orderVec[i] != "ACTIONORDER")
    {
        m_variablesToSend.push_back(orderVec[i++]);
    }
    if (i >= orderVec.size()) throw std::exception("Black box send wrong message: ACTIONORDER expected");
    i++;
    while (i < orderVec.size())
    {
        m_variablesToReceive.push_back(orderVec[i++]);
    }

    // testing can be done here later
    msgpack::sbuffer sbuffer;
    std::string data[1] = { std::to_string(p_amountOfTests) };
    msgpack::pack(sbuffer, data);
    m_server.SendData(sbuffer.data(), sbuffer.size());

    DecisionTuple decisionTuple;
    for (int i = 0; i < p_amountOfTests; i++)
    {
        sbuffer.clear();
        SerializeDriveSituation(sbuffer, p_tests[i]);
        m_server.SendData(sbuffer.data(), sbuffer.size());
        m_server.AwaitData(m_buffer, SBB_BUFFER_SIZE);
        DeserializeBlackBoxResults(m_buffer, SBB_BUFFER_SIZE, decisionTuple);
    }


    sbuffer.clear();
    SerializeDriveSituation(sbuffer, p_initialDriveSituation);
    m_server.SendData(sbuffer.data(), sbuffer.size());
    m_server.ReceiveDataAsync();
}

/// @brief Shuts the black box down
template <class DriveSituation>
void SocketBlackBox<DriveSituation>::Shutdown()
{
    m_server.AwaitData(m_buffer,SBB_BUFFER_SIZE);
    m_server.SendData("STOP", 4);
    m_server.AwaitData(m_buffer, SBB_BUFFER_SIZE);
    if (m_buffer[0] != 'O' || m_buffer[1] != 'K') throw std::exception("Client sent wrong reply");
    m_server.CloseServer();
}

/// @brief                  Inserts a string of value of variable in vector. Packs this vector to msgpack.
/// @param p_sbuffer        Buffer to pack data in
/// @param p_driveSituation Drive situation to serialize
template <class DriveSituation>
void SocketBlackBox<DriveSituation>::SerializeDriveSituation(msgpack::sbuffer& p_sbuffer, DriveSituation& p_driveSituation)
{
    std::vector<std::string> dataToSerialize;

    if (m_variablesToSend.empty())
    {
        dataToSerialize.push_back("There are no variables to send");
        msgpack::pack(p_sbuffer, dataToSerialize);
        throw std::exception("m_variablesToSend is empty");
    }

    for (auto i = m_variablesToSend.begin(); i != m_variablesToSend.end(); i++) {
        try { m_variableConvertAndInsertMap.at(*i)(dataToSerialize, p_driveSituation); }
        catch (std::exception& e)
        {
            dataToSerialize.push_back("Variable key does not exist");
        }
    }

    msgpack::pack(p_sbuffer, dataToSerialize);
}

/// @brief                Deserializes received data and makes a decision array from this data
/// @param p_decisions    Decision array to put decisions in
/// @param p_dataReceived Data received from black box
/// @param p_size         Size of received data
template <class DriveSituation>
void SocketBlackBox<DriveSituation>::DeserializeBlackBoxResults(const char* p_dataReceived, unsigned int p_size, DecisionTuple& p_decisionTuple)
{
    //unpack
    msgpack::unpacked msg;
    msgpack::unpack(msg, p_dataReceived, p_size);

    //convert
    msgpack::object obj = msg.get();
    std::vector<std::string> resultVec;
    obj.convert(resultVec);

    if (m_variablesToReceive.empty()) throw std::exception("No variables to receive");
    if (m_variablesToReceive.size() != resultVec.size())
        throw std::exception("Number of variables received does not match number of expected variables to receive");
    for (int i = 0; i < m_variablesToReceive.size(); i++)
    {
        try { m_variableDecisionMap.at(m_variablesToReceive[i])(resultVec.at(i), p_decisionTuple); }
        catch (std::exception& e) { throw e; }
    }
}

/// @brief Sends serialized drive situation to black box. Deserializes data received by black box.
///        Makes decisions from received data.
/// @param p_driveSituation Drive situation to base decisions off.
/// @return returns decision array.
template<class DriveSituation>
bool SocketBlackBox<DriveSituation>::GetDecisions(DriveSituation& p_driveSituation, DecisionTuple& p_decisions)
{
    if (!m_server.GetData(m_buffer, SBB_BUFFER_SIZE)) return false;
    msgpack::sbuffer sbuffer;
    SerializeDriveSituation(sbuffer, p_driveSituation);
    m_server.SendData(sbuffer.data(), sbuffer.size());
    m_server.ReceiveDataAsync();

    DeserializeBlackBoxResults(m_buffer, SBB_BUFFER_SIZE, p_decisions);

    return true;
}