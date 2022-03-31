#pragma once
#include "DecisionMaker.h"
#include "DecisionTuple.h"
#include "Mediator.h"
#include "ConfigEnums.h"


/// @brief  Creates an implementation of a decision maker
#define CREATE_DECISION_MAKER_IMPLEMENTATION(type1,type2) \
    template void DecisionMaker<type1,type2>::Initialize(DriveSituation& p_initialSituation,DriveSituation* p_testSituations, int p_testAmount);\
    template bool DecisionMaker<type1,type2>::Decide(DriveSituation& p_driveSituation);\
    template void DecisionMaker<type1,type2>::ChangeSettings(InterventionType p_type);\
    template void DecisionMaker<type1,type2>::SetDataCollectionSettings(tDataToStore p_dataSetting);\
    template DecisionMaker<type1, type2>::~DecisionMaker();

#define TEMP_DECISIONMAKER DecisionMaker<SocketBlackBox,SDAConfig>

/// @brief                     Initializes the decision maker
/// @param  p_initialSituation The initial situation
/// @param  p_testSituations   The test situations
/// @param  p_testAmount       The amount of tests
template <typename SocketBlackBox, typename SDAConfig>
void DecisionMaker<SocketBlackBox, SDAConfig>::Initialize(DriveSituation& p_initialSituation,
    DriveSituation* p_testSituations, int p_testAmount)
{
    m_recorder = new Recorder("BB_Recordings", "bbRecording", 2);
    BlackBox.Initialize(p_initialSituation, p_testSituations, p_testAmount);
}

/// @brief                   Tries to get a decision from the black box
/// @param  p_driveSituation The current drive situation
/// @return                  Whether a decision was made
template <typename SocketBlackBox, typename SDAConfig>
bool TEMP_DECISIONMAKER::Decide(DriveSituation& p_driveSituation)
{
    DecisionTuple decision;
    if (!BlackBox.GetDecisions(p_driveSituation, decision)) return false;

    int decisionCount = 0;
    IDecision** decisions = decision.GetActiveDecisions(decisionCount);

    InterventionExecutor->RunDecision(decisions, decisionCount);

#if defined(BB_RECORD_SESSION) && !defined(TEST)
    const float decisionValues[2] = { decision.GetBrake(), decision.GetSteer() };
    m_recorder->WriteRecording(decisionValues, p_driveSituation.GetTickCount(), false);
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
#ifndef TEST
    delete m_recorder;
#endif
}
