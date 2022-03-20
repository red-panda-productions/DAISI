#pragma once
#include "car.h"
#define UI_TYPE int
#define UI_TYPE_NONE 0
#define UI_TYPE_TURN_LEFT 1
#define UI_TYPE_TURN_RIGHT 2
#define UI_TYPE_BRAKE 3

/// @brief The car controller of the mediator
class CarController
{
public:
	void SetSteerCmd(float p_steer) const;

	void SetAccelCmd(float p_accel) const;

	void SetBrakeCmd(float p_brake) const;

	void SetClutchCmd(float p_clutch) const;

	void SetLightCmd(bool p_light) const;

	void ShowUI(UI_TYPE p_element) const;

	void SetCar(tCarElt* p_car);
private:
	tCarElt* m_car = nullptr;
};