#pragma once
#include "ConfigEnums.h"
#include "car.h"

/// @brief The car controller of the mediator
class CarController
{
public:
    CarController()
    {
        m_thresholds = new DecisionThresholds();
        m_thresholds->Steer = 0;
        m_thresholds->Brake = 0;
        m_thresholds->Accel = 0;
    }

    void SetSteerCmd(float p_steer);

    void SetAccelCmd(float p_accel);

    void SetBrakeCmd(float p_brake);

    void SetThresholds(tDecisionThresholds* p_thresholds);

    void SetClutchCmd(float p_clutch) const;

    void SetLightCmd(bool p_light) const;

    float GetSteerCmd() const;

    float GetAccelCmd() const;

    float GetBrakeCmd() const;

    float GetClutchCmd() const;

    int GetLightCmd() const;

    bool HasMadeSteerDecision() const;
    bool HasMadeBrakeDecision() const;
    bool HasMadeAccelDecision() const;

    void SetSteerDecision(bool p_steerDecision);
    void SetBrakeDecision(bool p_brakeDecision);
    void SetAccelDecision(bool p_accelDecision);

    void ShowIntervention(InterventionAction p_element) const;

    void SetCar(tCarElt* p_car);

    const tCarElt* GetCar() const;

private:
    tCarElt* m_car = nullptr;
    bool m_hasMadeSteerDecision = false;
    bool m_hasMadeBrakeDecision = false;
    bool m_hasMadeAccelDecision = false;

    tDecisionThresholds* m_thresholds = nullptr;
};