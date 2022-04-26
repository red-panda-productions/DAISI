#include <algorithm>
#include "CarController.h"
#include "IndicatorConfig.h"

/// @brief			Edits the steer command of the game
/// @param  p_steer The amount that needs to be edited
void CarController::SetSteerCmd(float p_steer) const
{
    m_car->ctrl.steer += p_steer;
}

/// @brief			Edits the acceleration command of the game
/// @param  p_accel The amount that needs to be edited
void CarController::SetAccelCmd(float p_accel) const
{
    m_car->ctrl.accelCmd += p_accel;
}

/// @brief			Edits the brake command of the game
/// @param  p_brake The amount that needs to be edited
void CarController::SetBrakeCmd(float p_brake) const
{
    m_car->ctrl.brakeCmd = std::max(p_brake, m_car->ctrl.brakeCmd);
}

/// @brief			 Edits the clutch command of the game
/// @param  p_clutch The amount that needs to be edited
void CarController::SetClutchCmd(float p_clutch) const
{
    m_car->ctrl.clutchCmd = p_clutch;
}

/// @brief			Edits the light command of the game
/// @param  p_light The amount that needs to be edited
void CarController::SetLightCmd(bool p_light) const
{
    m_car->ctrl.lightCmd = p_light;
}

/// @brief			  Shows an element on the UI
/// @param  p_element The element that should be shown
void CarController::ShowIntervention(InterventionAction p_element) const
{
    IndicatorConfig::GetInstance()->ActivateIndicator(p_element);
}

/// @brief		  Sets the current car of the game
/// @param  p_car The current car of the game
void CarController::SetCar(tCarElt* p_car)
{
    m_car = p_car;
}
float CarController::GetSteerCmd() const
{
    return m_car->ctrl.steer;
}
float CarController::GetAccelCmd() const
{
    return m_car->ctrl.accelCmd;
}
float CarController::GetBrakeCmd() const
{
    return m_car->ctrl.brakeCmd;
}
float CarController::GetClutchCmd() const
{
    return m_car->ctrl.clutchCmd;
}
int CarController::GetLightCmd() const
{
    return m_car->ctrl.lightCmd;
}
