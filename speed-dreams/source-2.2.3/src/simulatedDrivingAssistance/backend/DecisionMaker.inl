#pragma once
#include "DecisionMaker.h"
#include "DecisionTuple.h"
#include "Mediator.h"
#include "ConfigEnums.h"
#include "../rppUtils/RppUtils.hpp"

/// @brief  Creates an implementation of a decision maker
#define CREATE_DECISION_MAKER_IMPLEMENTATION(type1,type2) \
    template void DecisionMaker<type1,type2>::Initialize(tCarElt* p_initialCar, tSituation* p_initialSituation, BlackBoxData* p_testSituations, int p_testAmount);\
    template bool DecisionMaker<type1,type2>::Decide(tCarElt* p_car, tSituation* p_situation, int p_tickCount);\
    template void DecisionMaker<type1,type2>::ChangeSettings(InterventionType p_type);\
    template void DecisionMaker<type1,type2>::SetDataCollectionSettings(tDataToStore p_dataSetting);\
    template void DecisionMaker<type1,type2>::RaceStop();\
    template DecisionMaker<type1, type2>::~DecisionMaker();

#define TEMP_DECISIONMAKER DecisionMaker<SocketBlackBox,SDAConfig>

#define READ_FROM_FILE_SAFE(p_filestream, p_string) \
    if (p_filestream.eof())\
        {   p_filestream.close();\
            throw std::exception("Reached end of file prematurely"); \
        }\
    p_filestream >> p_string;

/// @brief                     Initializes the decision maker
/// @param  p_initialCar       The initial car
/// @param  p_initialSituation The initial situation
/// @param  p_testSituations   The test situations
/// @param  p_testAmount       The amount of tests
template <typename SocketBlackBox, typename SDAConfig>
void DecisionMaker<SocketBlackBox, SDAConfig>::Initialize(tCarElt* p_initialCar, 
    tSituation* p_initialSituation, BlackBoxData* p_testSituations, int p_testAmount)
{
#ifdef BB_RECORD_SESSION
    m_recorder = new Recorder("BB_Recordings", "bbRecording", 2);
#endif

    BlackBoxData initialData(p_initialCar, p_initialSituation, 0, nullptr, 0);
    BlackBox.Initialize(initialData, p_testSituations, p_testAmount);
}

/// @brief              Tries to get a decision from the black box
/// @param  p_car       The current car
/// @param  p_situation The current situation
/// @param  p_tickCount The current tick count
/// @return             Whether a decision was made
template <typename SocketBlackBox, typename SDAConfig>
bool TEMP_DECISIONMAKER::Decide(tCarElt* p_car, tSituation* p_situation, int p_tickCount)
{
    DecisionTuple decision;

    if (!BlackBox.GetDecisions(p_car, p_situation, p_tickCount, decision)) return false;

    int decisionCount = 0;
    IDecision** decisions = decision.GetActiveDecisions(decisionCount);

    InterventionExecutor->RunDecision(decisions, decisionCount);

#if defined(BB_RECORD_SESSION) && !defined(TEST)
    const float decisionValues[2] = { decision.GetBrake(), decision.GetSteer() };
    m_recorder->WriteRecording(decisionValues, p_tickCount, false);
#endif

    return true;
}

/// @brief                Changes the settings of how decisions should be made
/// @param  p_dataSetting The new type of interventions
template<typename SocketBlackBox, typename SDAConfig>
void TEMP_DECISIONMAKER::ChangeSettings(InterventionType p_dataSetting)
{
    InterventionExecutor = Config.SetInterventionType(p_dataSetting);
}

/// @brief         Changes the settings of how decisions should be made
/// @param  p_type The new type of interventions
template<typename SocketBlackBox, typename SDAConfig>
void TEMP_DECISIONMAKER::SetDataCollectionSettings(tDataToStore p_dataSetting)
{
    Config.SetDataCollectionSettings(p_dataSetting);
}

template<typename SocketBlackBox, typename SDAConfig>
DecisionMaker<SocketBlackBox, SDAConfig>::~DecisionMaker()
{
#ifdef BB_RECORD_SESSION
    delete m_recorder;
#endif
}

/// @brief When the race stops, the simulation data collected will be stored in the database
template<typename SocketBlackBox, typename SDAConfig>
void TEMP_DECISIONMAKER::RaceStop()
{
    std::string configPath(ROOT_FOLDER "\\data");
    std::string configFile("database_connection_settings.txt");

    if (!FindFileDirectory(configPath, configFile)) throw std::exception("Could not find database settings file");

    std::ifstream ifstream(configPath + '\\' + configFile);
    if (ifstream.fail()) throw std::exception("Could not open database settings file");

    std::string ip;
    std::string portString;
    std::string username;
    std::string password;
    std::string schema;

    READ_FROM_FILE_SAFE(ifstream, ip);
    READ_FROM_FILE_SAFE(ifstream, portString);
    READ_FROM_FILE_SAFE(ifstream, username);
    READ_FROM_FILE_SAFE(ifstream, password);
    READ_FROM_FILE_SAFE(ifstream, schema);

    ifstream.close();

    int port;
    try {port = std::stoi(portString);}
    catch (std::exception& e) { throw std::exception("Port in database settings config file could not be converted to an int");}

    if (m_SQLDatabaseStorage.OpenDatabase(ip, port, username, password, schema))
    {
        //m_SQLDatabaseStorage.StoreData([INPUT FILE PATH HERE]);
        m_SQLDatabaseStorage.CloseDatabase();
    }
}
