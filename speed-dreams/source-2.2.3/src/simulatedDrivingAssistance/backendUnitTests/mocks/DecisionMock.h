#pragma once
#include "Decision.h"

class DecisionMock : public Decision
{
public:
    DecisionMock()
    {
        SetInterventionAmount(1);
    }

    // @brief    increases the indicate integer variable by 1
    void ShowIntervention(float p_interventionAmount) override
    {
        Indicate++;
    }

    // always reach the threshold
    bool ReachThreshold(float p_interventionAmount) override
    {
        return true;
    }

    // always intervene
    bool CanIntervene(tAllowedActions p_allowedActions) override
    {
        return true;
    }

    // @brief    increases the intervene integer variable by 1
    void DoIntervention(float p_interventionAmount) override
    {
        Intervene++;
    }

    int Indicate = 0;   // int variable that checks if an indicator should be shown
    int Intervene = 0;  // int variable that checks if an intervention should happen
};