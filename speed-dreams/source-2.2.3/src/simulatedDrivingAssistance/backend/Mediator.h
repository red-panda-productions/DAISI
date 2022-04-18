#pragma once
#include "ConfigEnums.h"
#include "car.h"
#include "raceman.h"
#include "DecisionMaker.h"
#include "CarController.h"

/// @brief The standard type of the mediator
#define SMediator Mediator<SDecisionMaker>

/// @brief			      The Main communication between the front- and backend
/// @tparam DecisionMaker The decisionMaker type
template <typename DecisionMaker>
class Mediator
{
public:
    void DriveTick(tCarElt* p_car, tSituation* p_situation);
    void RaceStart(tTrack* p_track, void* p_carHandle, void** p_carParmHandle, tSituation* p_situation);
    void RaceStop();

    void SetTask(Task p_task);
    void SetIndicatorSettings(tIndicator p_indicators);
    void SetInterventionType(InterventionType p_type);
    void SetPControlSettings(tParticipantControl p_pControl);

    void SetMaxTime(int p_maxTime);
    void SetUserId(char* p_userId);

    void SetDataCollectionSettings(tDataToStore p_dataSetting);

    void SetBlackBoxFilePath(const char* p_filePath);

    tIndicator          GetIndicatorSettings();
    InterventionType    GetInterventionType();
    tParticipantControl GetPControlSettings();

    static Mediator* GetInstance();

    /// @brief Removes copy constructor for singleton behaviour
    Mediator(Mediator const&) = delete;
    /// @brief Removes assigment for singleton behaviour
    void operator=(Mediator const&) = delete;

    CarController CarController;

private:
    Mediator() = default;
    static Mediator<DecisionMaker>* m_instance;

    DecisionMaker m_decisionMaker;

    unsigned long m_tickCount = 0;

    tTrack* m_track = nullptr;
};

template <>
SMediator* SMediator::m_instance = nullptr;