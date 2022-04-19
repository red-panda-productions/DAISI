#pragma once
#include "ConfigEnums.h"
#include "raceman.h"
#include "car.h"

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
    }

    InterventionType Type;
    bool Decision;

    ~DecisionMakerMock() = default;
};