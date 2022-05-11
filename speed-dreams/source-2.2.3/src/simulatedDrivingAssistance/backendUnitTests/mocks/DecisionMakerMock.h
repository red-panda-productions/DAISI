#pragma once
#include "ConfigEnums.h"
#include "raceman.h"
#include "FileDataStorageMock.h"
#include "car.h"
#include "ConfigMock.h"

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

    InterventionType Type;
    FileDataStorageMock FileBufferStorage;
    bool Decision;

    ConfigMock Config;

    ~DecisionMakerMock() = default;
};

/// @brief The standard DecisionMakerMock
#define SDecisionMakerMock DecisionMakerMock<ConfigMock>