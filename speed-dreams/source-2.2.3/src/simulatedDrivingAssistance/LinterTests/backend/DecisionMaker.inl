#pragma once
#include "DecisionMaker.h"
#include "DecisionTuple.h"
#include "Mediator.h"
#include "ConfigEnums.h"
#include "../rppUtils/RppUtils.hpp"

/// @brief  Creates an implementation of a decision maker
#define CREATE_DECISION_MAKER_IMPLEMENTATION(type1, type2)                                                                 \
    template void DecisionMaker<type1, type2>::Initialize(tCarElt* p_initialCar,                                           \
                                                          tSituation* p_initialSituation,                                  \
                                                          const std::string& p_blackBoxExecutablePath,                     \
                                                          bool p_recordBB,                                                 \
                                                          BlackBoxData* p_testSituations,                                  \
                                                          int p_testAmount);                                               \
    template bool DecisionMaker<type1, type2>::Decide(tCarElt* p_car, tSituation* p_situation, unsigned long p_tickCount); \
    template void DecisionMaker<type1, type2>::ChangeSettings(InterventionType p_dataSetting);                             \
    template void DecisionMaker<type1, type2>::SetDataCollectionSettings(tDataToStore p_dataSetting);                      \
    template void DecisionMaker<type1, type2>::RaceStop();                                                                 \
    template DecisionMaker<type1, type2>::~DecisionMaker();

#define TEMP_DECISIONMAKER DecisionMaker<SocketBlackBox, SDAConfig>

/// @brief                     Initializes the decision maker
/// @param  p_initialCar       The initial car
/// @param  p_initialSituation The initial situation
/// @param  p_recordBB         If the blackbox decisions will be recorded
/// @param  p_blackBoxExecutablePath The path to the black box executable.
/// Should either be an absolute path or relative to the current directory.
/// Path must include file extension; no default extension is assumed.
/// Path is assumed to refer to an existing executable file
/// @param  p_testSituations   The test situations
/// @param  p_testAmount       The amount of tests
template <typename SocketBlackBox, typename SDAConfig>
void DecisionMaker<SocketBlackBox, SDAConfig>::Initialize(tCarElt* p_initialCar,
                                                          tSituation* p_initialSituation,
                                                          const std::string& p_blackBoxExecutablePath,
                                                          bool p_recordBB,
                                                          BlackBoxData* p_testSituations,
                                                          int p_testAmount)
{
#if !defined(TEST)
    if (p_recordBB)
    {
        m_recorder = new Recorder("BB_Recordings", "bbRecording", 2);
    }
#endif

    StartExecutable(p_blackBoxExecutablePath);

    BlackBoxData initialData(p_initialCar, p_initialSituation, 0, nullptr, 0);
    BlackBox.Initialize(initialData, p_testSituations, p_testAmount);
}

/// @brief              Tries to get a decision from the black box
/// @param  p_car       The current car
/// @param  p_situation The current situation
/// @param  p_tickCount The current tick count
/// @return             Whether a decision was made
template <typename SocketBlackBox, typename SDAConfig>
bool TEMP_DECISIONMAKER::Decide(tCarElt* p_car, tSituation* p_situation, unsigned long p_tickCount)
{
    DecisionTuple decision;

    if (!BlackBox.GetDecisions(p_car, p_situation, p_tickCount, decision)) return false;

    int decisionCount = 0;
    IDecision** decisions = decision.GetActiveDecisions(decisionCount);

    InterventionExecutor->RunDecision(decisions, decisionCount);

#if !defined(TEST)
    if (m_recorder)
    {
        const float decisionValues[2] = {decision.GetBrake(), decision.GetSteer()};
        m_recorder->WriteRecording(decisionValues, p_tickCount, false);
    }
#endif

    return true;
}

/// @brief                Changes the settings of how decisions should be made
/// @param  p_dataSetting The new type of interventions
template <typename SocketBlackBox, typename SDAConfig>
void TEMP_DECISIONMAKER::ChangeSettings(InterventionType p_dataSetting)
{
    InterventionExecutor = Config.SetInterventionType(p_dataSetting);
}

/// @brief         Changes the settings of what data should be collected
/// @param  p_type The new data collection settings
template <typename SocketBlackBox, typename SDAConfig>
void TEMP_DECISIONMAKER::SetDataCollectionSettings(tDataToStore p_dataSetting)
{
    Config.SetDataCollectionSettings(p_dataSetting);
}

template <typename SocketBlackBox, typename SDAConfig>
DecisionMaker<SocketBlackBox, SDAConfig>::~DecisionMaker()
{
    delete m_recorder;
}

/// @brief When the race stops, the simulation data collected will be stored in the database
template <typename SocketBlackBox, typename SDAConfig>
void TEMP_DECISIONMAKER::RaceStop()
{
    BlackBox.Shutdown();
    m_sqlDatabaseStorage.Run("INPUT FILE HERE");
}
