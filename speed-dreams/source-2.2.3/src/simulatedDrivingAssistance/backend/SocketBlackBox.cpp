#include "SocketBlackBox.h"
#include <string>
#include "SteerDecision.h"
#include "BrakeDecision.h"

// inserts value from DriveSituation variables in vector
#define PUSH_BACK_DS(GetInfo) [](std::vector<std::string>& values, DriveSituation& ds){values.push_back(std::to_string(ds.GetInfo));}
#define INSERT_CAR_INFO(variable) PUSH_BACK_DS(GetCarInfo().variable)
#define INSERT_ENVIRONMENT_INFO(variable) PUSH_BACK_DS(GetEnvironmentInfo().variable)
#define INSERT_TRACK_LOCAL_POSITION(variable) INSERT_ENVIRONMENT_INFO(TrackLocalPosition().variable)
#define INSERT_PLAYER_INFO(variable) PUSH_BACK_DS(GetPlayerInfo().variable)

// converts abstract decision to concrete decision with correct value
#define DECISION_LAMBDA(f) [](std::string& string, IDecision& decision) {f;}
#define CONVERT_TO_STEER_DECISION DECISION_LAMBDA(dynamic_cast<SteerDecision&>(decision).m_steerAmount = std::stof(string))
#define CONVERT_TO_BRAKE_DECISION DECISION_LAMBDA(dynamic_cast<BrakeDecision&>(decision).m_brakeAmount = std::stof(string))

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
void SocketBlackBox<DriveSituation>::DeserializeBlackBoxResults(IDecision* decisions, const char* dataReceived, unsigned int size)
{
    //unpack
    msgpack::unpacked msg;
    msgpack::unpack(msg, dataReceived, size);

    //convert
    msgpack::object obj = msg.get();
    std::vector<std::string> resultVec;
    obj.convert(resultVec);

    for (int i = 0; i < m_variablesToReceive.size(); i++)
    {
        IDecision* decision;
        m_variableDecisionMap.at(m_variablesToReceive[i])(resultVec.at(i), *decision) ;
        decisions[i] = *decision;
    }
}

/// @brief Sends serialized drive situation to black box. Deserializes data received by black box.
///        Makes decisions from received data.
/// @param driveSituation Drive situation to base decisions off.
/// @return returns decision array.
template<class DriveSituation>
IDecision* SocketBlackBox<DriveSituation>::GetDecisions(DriveSituation& driveSituation)
{
    std::stringstream stringstream;
    SerializeDriveSituation(stringstream, driveSituation);

    return nullptr;

    /*TODO: send and receive data with IPCLib*/
//    IDecision* decisions;
//    DeserializeBlackBoxResults(decisions, dataReceived, dataSize);
//    return decisions;
}

