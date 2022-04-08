#pragma once
#include "DecisionMaker.h"
#include "DecisionTuple.h"
#include "Mediator.h"
#include "/../rppUtils/RppUtils.hpp"

/// @brief  Creates an implementation of a decision maker
#define CREATE_DECISION_MAKER_IMPLEMENTATION(type1,type2) \
    template void DecisionMaker<type1,type2>::Initialize(DriveSituation& p_initialSituation,DriveSituation* p_testSituations, int p_testAmount);\
    template bool DecisionMaker<type1,type2>::Decide(DriveSituation& p_driveSituation);\
    template void DecisionMaker<type1,type2>::ChangeSettings(InterventionType p_type);\
    template void DecisionMaker<type1,type2>::SetDataCollectionSettings(tDataToStore p_dataSetting);\
    template void DecisionMaker<type1,type2>::RaceStop();

#define TEMP_DECISIONMAKER DecisionMaker<SocketBlackBox,SDAConfig>

/// @brief                     Initializes the decision maker
/// @param  p_initialSituation The initial situation
/// @param  p_testSituations   The test situations
/// @param  p_testAmount       The amount of tests
template <typename SocketBlackBox, typename SDAConfig>
void DecisionMaker<SocketBlackBox, SDAConfig>::Initialize(DriveSituation& p_initialSituation,
    DriveSituation* p_testSituations, int p_testAmount)
{
    m_blackBox.Initialize(p_initialSituation, p_testSituations, p_testAmount);
}

/// @brief                   Tries to get a decision from the black box
/// @param  p_driveSituation The current drive situation
/// @return                  Whether a decision was made
template <typename SocketBlackBox, typename SDAConfig>
bool TEMP_DECISIONMAKER::Decide(DriveSituation& p_driveSituation)
{
    DecisionTuple decision;
    if (!m_blackBox.GetDecisions(p_driveSituation, decision)) return false;

    int decisionCount = 0;
    IDecision** decisions = decision.GetActiveDecisions(decisionCount);
    m_interventionExecutor->RunDecision(decisions, decisionCount);
    return true;
}

/// @brief                Changes the settings of how decisions should be made
/// @param  p_dataSetting The new type of interventions
template<typename SocketBlackBox, typename SDAConfig>
void TEMP_DECISIONMAKER::ChangeSettings(InterventionType p_dataSetting)
{
    m_interventionExecutor = Config.SetInterventionType(p_dataSetting);
}

/// @brief         Changes the settings of how decisions should be made
/// @param  p_type The new type of interventions
template<typename SocketBlackBox, typename SDAConfig>
void TEMP_DECISIONMAKER::SetDataCollectionSettings(tDataToStore p_dataSetting)
{
    Config.SetDataCollectionSettings(p_dataSetting);
}

template<typename SocketBlackBox, typename SDAConfig>
void TEMP_DECISIONMAKER::RaceStop()
{
    std::string configPath("source-2.2.3\\data\\database_connection_settings.txt");

    if (!FindFileDirectory(configPath, "")) throw std::exception("Could not find database settings file");

    std::ifstream ifstream(configPath);

    std::string ip;
    std::string portString;
    std::string username;
    std::string password;
    std::string schema;

    try
    {
        ifstream >> ip;
        ifstream >> portString;
        ifstream >> username;
        ifstream >> password;
        ifstream >> schema;
    }
    catch (std::exception& e) { throw std::exception("Something went wrong while reading the config file");}

    int port;
    try {std::stoi(portString);}
    catch (std::exception& e) { throw std::exception("Port in database settings config file could not be converted to an int");}

    m_SQLDatabaseStorage.OpenDatabase(ip, port, username, password, schema);
    m_SQLDatabaseStorage.StoreData("");
    m_SQLDatabaseStorage.CloseDatabase();
}