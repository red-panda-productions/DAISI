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

	void ShowUI(InterventionAction p_element) const;

	void SetCar(tCarElt* p_car);
private:
	tCarElt* m_car = nullptr;
};