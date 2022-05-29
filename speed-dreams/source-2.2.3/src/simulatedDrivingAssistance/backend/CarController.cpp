#include <algorithm>
#include <iostream>
#include "CarController.h"
#include "IndicatorConfig.h"

/// @brief			Edits the steer command of the game
/// @param  p_steer The amount that needs to be edited
void CarController::SetSteerCmd(float p_steer)
{
    m_car->ctrl.steer += p_steer;
    m_isSteerDecision = abs(p_steer) > m_thresholds->Steer;
}

/// @brief			Edits the acceleration command of the game
/// @param  p_accel The amount that needs to be edited
void CarController::SetAccelCmd(float p_accel)
{
    m_car->ctrl.accelCmd += p_accel;
    m_isAccelDecision = p_accel > m_thresholds->Accel;
}

/// @brief			Edits the brake command of the game
/// @param  p_brake The amount that needs to be edited
void CarController::SetBrakeCmd(float p_brake)
{
    m_car->ctrl.brakeCmd = std::max(p_brake, m_car->ctrl.brakeCmd);
    m_isBrakeDecision = p_brake > m_thresholds->Brake;
}

/// @brief			Sets the thresholds of the game
/// @param  p_steer The amount that needs to be edited
void CarController::SetThresholds(tDecisionThresholds* p_thresholds)
{
    m_thresholds = p_thresholds;
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

/// @brief  Gets whether there is a steer decision for the car
/// @return Whether there is a steer decision for the car
bool CarController::IsSteerDecision() const
{
    return m_isSteerDecision;
}

/// @brief  Gets whether there is a brake decision for the car
/// @return Whether there is a brake decision for the car
bool CarController::IsBrakeDecision() const
{
    return m_isBrakeDecision;
}

/// @brief  Gets whether there is an accel decision for the car
/// @return Whether there is an accel decision for the car
bool CarController::IsAccelDecision() const
{
    return m_isAccelDecision;
}

/// @brief Sets whether there is a steer decision for the car
/// @param p_steerDecision Whether there is a steer decision for the car
void CarController::SetSteerDecision(bool p_steerDecision)
{
    m_isSteerDecision = p_steerDecision;
}

/// @brief Sets whether there is a brake decision for the car
/// @param p_brakeDecision Whether there is a brake decision for the car
void CarController::SetBrakeDecision(bool p_brakeDecision)
{
    m_isBrakeDecision = p_brakeDecision;
}

/// @brief Sets whether there is an accel decision for the car
/// @param p_accelDecision Whether there is an accel decision for the car
void CarController::SetAccelDecision(bool p_accelDecision)
{
    m_isAccelDecision = p_accelDecision;
}