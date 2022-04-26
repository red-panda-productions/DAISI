#pragma once
#include "ConfigEnums.h"
#include "car.h"

/// @brief The car controller of the mediator
class CarController
{
public:
    void SetSteerCmd(float p_steer) const;

    void SetAccelCmd(float p_accel) const;

    void SetBrakeCmd(float p_brake) const;

    void SetClutchCmd(float p_clutch) const;

    void SetLightCmd(bool p_light) const;

    float GetSteerCmd() const;

    float GetAccelCmd() const;

    float GetBrakeCmd() const;

    float GetClutchCmd() const;

    int GetLightCmd() const;

    void ShowIntervention(InterventionAction p_element) const;

    void SetCar(tCarElt* p_car);

private:
    tCarElt* m_car = nullptr;
};