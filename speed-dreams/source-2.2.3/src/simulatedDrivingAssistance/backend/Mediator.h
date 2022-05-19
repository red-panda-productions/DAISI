#pragma once
#include "ConfigEnums.h"
#include "car.h"
#include "raceman.h"
#include "DecisionMaker.h"
#include "CarController.h"
#include "Recorder.h"

/// @brief			      The Main communication between the front- and backend
/// @tparam DecisionMaker The decisionMaker type
template <typename DecisionMaker>
class Mediator
{
public:
    void DriveTick(tCarElt* p_car, tSituation* p_situation);
    void RaceStart(tTrack* p_track, void* p_carHandle, void** p_carParmHandle, tSituation* p_situation, Recorder* p_recorder = nullptr);
    void RaceStop();
    bool TimeOut();

    void SetAllowedActions(tAllowedActions p_allowedActions);
    void SetIndicatorSettings(tIndicator p_indicators);
    void SetInterventionType(InterventionType p_type);
    void SetPControlSettings(tParticipantControl p_pControl);
    void SetReplayRecorderSetting(bool p_replayRecorderOn);
    void SetBlackBoxSyncOption(bool p_sync);

    void SetThresholdSettings(tDecisionThresholds p_thresholds);
    void SetMaxTime(int p_maxTime);

    void SetUserId(char* p_userId);

    void SetCompressionLevel(int p_compressionLevel);

    void SetDataCollectionSettings(tDataToStore p_dataSetting);

    void SetBlackBoxFilePath(const char* p_filePath);

    void SetReplayFolder(const filesystem::path& p_replayFolder);
    const filesystem::path& GetReplayFolder() const;

    tAllowedActions GetAllowedActions();
    tIndicator GetIndicatorSettings();
    InterventionType GetInterventionType();
    tParticipantControl GetPControlSettings();
    tDecisionThresholds GetThresholdSettings();
    bool GetReplayRecorderSetting();
    bool GetBlackBoxSyncOption();
    int GetMaxTime();

    static Mediator* GetInstance();
#ifdef TEST
    /// @brief          Clears the mediator instance
    /// @param p_delete Whether to delete the de mediator
    static void ClearInstance(bool p_delete = true)
    {
        if (p_delete) delete m_instance;
        m_instance = nullptr;
    }

    DecisionMaker* GetDecisionMaker()
    {
        return &m_decisionMaker;
    }

    /// @brief             Sets the tick count of the mediator
    /// @param p_tickCount The tick count
    void SetTickCount(unsigned long p_tickCount)
    {
        m_tickCount = p_tickCount;
    }

    /// @brief  Gets the tick count of the mediator
    /// @return The tick count
    unsigned long GetTickCount()
    {
        return m_tickCount;
    }

    /// @brief         Sets the in race state of the mediator
    /// @bool p_inRace Whether the game is in a race
    void SetInRace(bool p_inRace)
    {
        m_inRace = p_inRace;
    }

    /// @brief  Gets the race state of the mediator
    /// @return Whether the game is in a race
    bool GetInRace()
    {
        return m_inRace;
    }
#endif

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

    tDecisionThresholds m_thresholds;

    bool m_inRace = false;  // Whether the game is currently in a race
};

/// @brief The standard type of the mediator
#define SMediator Mediator<SDecisionMaker>

template <>
SMediator* SMediator::m_instance = nullptr;