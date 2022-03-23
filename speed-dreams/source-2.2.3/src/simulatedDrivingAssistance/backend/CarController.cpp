#include "CarController.h"
#include "InterventionConfig.h"

void CarController::SetSteerCmd(float p_steer) const
{
	m_car->ctrl.steer += p_steer;
}

void CarController::SetAccelCmd(float p_accel) const
{
	m_car->ctrl.accelCmd = p_accel;
}

void CarController::SetBrakeCmd(float p_brake) const
{
	m_car->ctrl.brakeCmd = std::max(p_brake, m_car->ctrl.brakeCmd);
}

void CarController::SetClutchCmd(float p_clutch) const
{
	m_car->ctrl.clutchCmd = p_clutch;
}

void CarController::SetLightCmd(bool p_light) const
{
	m_car->ctrl.lightCmd = p_light;
}

/// @brief			  Shows an element on the UI
/// @param  p_element The element that should be shown
void CarController::ShowUI(InterventionAction p_element) const
{
	InterventionConfig::GetInstance()->SetInterventionAction(p_element);
}

void CarController::SetCar(tCarElt* p_car)
{
	m_car = p_car;
}
