#pragma once
#include "ConfigEnums.h"
#include "raceman.h"
#include "FileDataStorageMock.h"
#include "car.h"
#include "ConfigMock.h"
#include "Recorder.h"

template <class ConfigMock>
class DecisionMakerMock
{
public:
    bool Decide(tCarElt* p_car, tSituation* p_situation, unsigned long p_tickCount) const
    {
        return Decision;
    }

    void ChangeSettings(InterventionType p_type)
    {
        Type = p_type;
        Config.SetInterventionType(p_type);
    }

    void SetDataCollectionSettings(tDataToStore p_dataSetting)
    {
        Config.SetDataCollectionSettings(p_dataSetting);
    }

    void Initialize(unsigned long p_initialTickCount, tCarElt* p_initialCar, tSituation* p_initialSituation, tTrack* p_track, const std::string& p_blackBoxExecutablePath,
                    Recorder* p_recorder, BlackBoxData* p_testSituations = nullptr, int p_testAmount = 0)
    {
    }

    void RaceStop(bool p_saveToDatabase)
    {
        MStoppedRace = true;
    }

    bool MStoppedRace;
    InterventionType Type;
    FileDataStorageMock FileBufferStorage;
    bool Decision;

    ConfigMock Config;

    ~DecisionMakerMock() = default;
};

/// @brief The standard DecisionMakerMock
#define SDecisionMakerMock DecisionMakerMock<ConfigMock>