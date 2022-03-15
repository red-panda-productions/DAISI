#pragma once

#define UI_TYPE int
#define UI_TYPE_NONE 0
#define UI_TYPE_TURN_LEFT 1
#define UI_TYPE_TURN_RIGHT 2
#define UI_TYPE_BRAKE 3

class CarController
{
public:
	void SetSteerCmd(float p_steer);

	void SetAccelCmd(float p_accel);

	void SetBrakeCmd(float p_brake);

	void SetClutchCmd(float p_clutch);

	void SetLightCmd(bool p_light);

	void ShowUI(UI_TYPE p_element);
};