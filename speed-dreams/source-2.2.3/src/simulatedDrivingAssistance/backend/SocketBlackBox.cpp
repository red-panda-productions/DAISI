#include "SocketBlackBox.h"
#include <string>
#include "../rppUtils/Utils.hpp"

// inserts value from DriveSituation variables in vector
#define PUSH_BACK_DS(p_GetInfo) [](std::vector<std::string>& p_values, DriveSituation& p_ds){p_values.push_back(std::to_string(p_ds.p_GetInfo));}
#define INSERT_CAR_INFO(p_variable) PUSH_BACK_DS(GetCarInfo().p_variable)
#define INSERT_ENVIRONMENT_INFO(p_variable) PUSH_BACK_DS(GetEnvironmentInfo().p_variable)
#define INSERT_TRACK_LOCAL_POSITION(p_variable) INSERT_ENVIRONMENT_INFO(TrackLocalPosition().p_variable)
#define INSERT_PLAYER_INFO(p_variable) PUSH_BACK_DS(GetPlayerInfo().p_variable)

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
    m_variableConvertAndInsertMap["AccelCmd"] = INSERT_CAR_INFO(AccelCmd());
    m_variableConvertAndInsertMap["BrakeCmd"] = INSERT_CAR_INFO(BrakeCmd());
    m_variableConvertAndInsertMap["ClutchCmd"] = INSERT_CAR_INFO(ClutchCmd());
    m_variableConvertAndInsertMap["SteerCmd"] = INSERT_CAR_INFO(SteerCmd());
    m_variableConvertAndInsertMap["TopSpeed"] = INSERT_CAR_INFO(TopSpeed());

    //EnvironmentInfo functions
    m_variableConvertAndInsertMap["TimeOfDay"] = INSERT_ENVIRONMENT_INFO(TimeOfDay());
    m_variableConvertAndInsertMap["Clouds"] = INSERT_ENVIRONMENT_INFO(Clouds());
    m_variableConvertAndInsertMap["Offroad"] = INSERT_ENVIRONMENT_INFO(Offroad());

    //TrackLocalPosition functions
    m_variableConvertAndInsertMap["ToMiddle"] = INSERT_TRACK_LOCAL_POSITION(ToMiddle());
    m_variableConvertAndInsertMap["ToLeft"] = INSERT_TRACK_LOCAL_POSITION(ToLeft());
    m_variableConvertAndInsertMap["ToRight"] = INSERT_TRACK_LOCAL_POSITION(ToRight());
    m_variableConvertAndInsertMap["ToStart"] = INSERT_TRACK_LOCAL_POSITION(ToStart());

    //PlayerInfo functions
    m_variableConvertAndInsertMap["TimeLastSteer"] = INSERT_PLAYER_INFO(TimeLastSteer());


    //Decision functions
    m_variableDecisionMap["Steer"] = CONVERT_TO_STEER_DECISION;
    m_variableDecisionMap["Brake"] = CONVERT_TO_BRAKE_DECISION;
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

/// @brief              Deserializes received data and makes a decision array from this data
/// @param decisions    Decision array to put decisions in
/// @param dataReceived Data received from black box
/// @param size         Size of received data
template <class DriveSituation>
void SocketBlackBox<DriveSituation>::DeserializeBlackBoxResults(DecisionTuple& p_decisionTuple, const char* p_dataReceived, unsigned int p_size)
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
        try {m_variableDecisionMap.at(m_variablesToReceive[i])(resultVec.at(i), p_decisionTuple);}
        catch (std::exception& e) {throw e;}
    }
}

/// @brief Sends serialized drive situation to black box. Deserializes data received by black box.
///        Makes decisions from received data.
/// @param driveSituation Drive situation to base decisions off.
/// @return returns decision array.
template<class DriveSituation>
DecisionTuple SocketBlackBox<DriveSituation>::GetDecisions(DriveSituation& p_driveSituation)
{
    std::stringstream stringstream;
    SerializeDriveSituation(stringstream, p_driveSituation);

    DecisionTuple decisionTuple();

    /*TODO: send and receive data with IPCLib*/

    return decisionTuple();
}

