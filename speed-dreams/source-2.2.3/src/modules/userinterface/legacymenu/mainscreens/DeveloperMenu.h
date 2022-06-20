/*
 * This program has been developed by students from the bachelor Computer Science at
 * Utrecht University within the Software Project course.
 * © Copyright Utrecht University (Department of Information and Computing Sciences)
 */

#pragma once
#include "ConfigEnums.h"

extern void* DeveloperMenuInit(void* p_prevMenu);
extern void DeveloperMenuRun(void*);
extern void ConfigureDeveloperSettings();
extern void RemoteSetDefaultThresholdValues();
extern void SetTempInterventionType(InterventionType p_interventionType);
