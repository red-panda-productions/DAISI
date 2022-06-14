#include <algorithm>
#include <iostream>
#include "CarController.h"
#include "IndicatorConfig.h"

/// @brief			Edits the steer command of the game
/// @param  p_steer The amount that needs to be edited
void CarController::SetSteerCmd(float p_steer)
{
    m_car->ctrl.steer += p_steer;
}

/// @brief			Edits the acceleration command of the game
/// @param  p_accel The amount that needs to be edited
void CarController::SetAccelCmd(float p_accel)
{
    m_car->ctrl.accelCmd = std::max(p_accel, m_car->ctrl.accelCmd);
}

/// @brief			Edits the brake command of the game
/// @param  p_brake The amount that needs to be edited
void CarController::SetBrakeCmd(float p_brake)
{
    m_car->ctrl.brakeCmd = std::max(p_brake, m_car->ctrl.brakeCmd);
}

/// @brief			 Edits the clutch command of the game
/// @param  p_clutch The amount that needs to be edited
void CarController::SetClutchCmd(float p_clutch) const
{
    m_car->ctrl.clutchCmd += p_clutch;
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

/// @brief  Gets the car pointer
/// @return The car pointer
const tCarElt* CarController::GetCar() const
{
    return m_car;
}

/// @brief  Gets the steer value of the car
/// @return The steer value of the car
float CarController::GetSteerCmd() const
{
    return m_car->ctrl.steer;
}

/// @brief  Gets the acceleration value of the car
/// @return The acceleration value of the car
float CarController::GetAccelCmd() const
{
    return m_car->ctrl.accelCmd;
}

/// @brief  Gets the brake value of the car
/// @return The brake value of the car
float CarController::GetBrakeCmd() const
{
    return m_car->ctrl.brakeCmd;
}

/// @brief  Gets the clutch value of the car
/// @return The clutch value of the car
float CarController::GetClutchCmd() const
{
    return m_car->ctrl.clutchCmd;
}

/// @brief  Gets the light value of the car
/// @return The light value of the car
int CarController::GetLightCmd() const
{
    return m_car->ctrl.lightCmd;
}