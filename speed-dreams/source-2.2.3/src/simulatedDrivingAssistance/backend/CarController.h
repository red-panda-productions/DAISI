#pragma once
#include "ConfigEnums.h"
#include "car.h"

/// @brief The car controller of the mediator
class CarController
{
public:
    void SetSteerCmd(float p_steer);
    void SetAccelCmd(float p_accel);
    void SetBrakeCmd(float p_brake);
    void SetClutchCmd(float p_clutch) const;
    void SetLightCmd(bool p_light) const;

    float GetSteerCmd() const;
    float GetAccelCmd() const;
    float GetBrakeCmd() const;
    float GetClutchCmd() const;
    int GetLightCmd() const;

    void ShowIntervention(InterventionAction p_element) const;

    void SetCar(tCarElt* p_car);
    const tCarElt* GetCar() const;

private:
    tCarElt* m_car = nullptr;
};