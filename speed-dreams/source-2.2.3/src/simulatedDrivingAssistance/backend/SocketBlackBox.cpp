#include "SocketBlackBox.h"
#include "msgpack.hpp"
#include <string>

// inserts value from DriveSituation variables in vector
#define PUSH_BACK_DS(GetInfo) [](std::vector<std::string>& values, DriveSituation& ds){values.push_back(std::to_string(ds.GetInfo
#define INSERT_CAR_INFO(variable) PUSH_BACK_DS(GetCarInfo().variable)));}
#define INSERT_ENVIRONMENT_INFO(variable) PUSH_BACK_DS(GetEnvironmentInfo().variable)));}
#define INSERT_TRACK_LOCAL_POSITION(variable) INSERT_ENVIRONMENT_INFO(TrackLocalPosition().variable)
#define INSERT_PLAYER_INFO(variable) PUSH_BACK_DS(GetPlayerInfo().variable)));}

void SocketBlackBox::Initialize()
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


//    //Decision functions
//    m_variableDecisionMap["Steer"] =
}

void SocketBlackBox::SerializeDriveSituation(std::stringstream& stringstream, DriveSituation& driveSituation)
{
    std::vector<std::string> dataToSerialize;

    for (auto i = m_variablesToSend.begin(); i != m_variablesToSend.end(); i++)
    {
        m_variableConvertAndInsertMap.at(*i)(dataToSerialize, driveSituation);
    }

    msgpack::pack(stringstream, dataToSerialize);
}

//void DeserializeBlackBoxResults(const char* dataReceived, unsigned int size)
//{
//    //unpack
//    msgpack::unpacked msg;
//    msgpack::unpack(msg, dataReceived, size);
//
//    //convert
//    msgpack::object obj = msg.get();
//    std::vector<std::string> resultVec;
//    obj.convert(resultVec);
//
//
//
//}

void SocketBlackBox::GetDecisions(DriveSituation& driveSituation)
{
    std::stringstream stringstream;
    SerializeDriveSituation(stringstream, driveSituation);

//    const char* dataReceived;
//    std::size_t dataSize;
//    DeserializeBlackBoxResults(dataReceived, dataSize);
}

