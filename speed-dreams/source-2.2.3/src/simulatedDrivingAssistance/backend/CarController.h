#pragma once

#include "ConfigEnums.h"

/// @brief The car controller of the mediator
class CarController
{
public:
	void SetSteerCmd(float p_steer);

	void SetAccelCmd(float p_accel);

	void SetBrakeCmd(float p_brake);

	void SetClutchCmd(float p_clutch);

	void SetLightCmd(bool p_light);

	void ShowUI(InterventionAction p_element);
};