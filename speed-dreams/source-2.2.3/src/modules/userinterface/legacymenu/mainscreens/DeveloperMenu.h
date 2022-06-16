#pragma once
#include "ConfigEnums.h"

extern void* DeveloperMenuInit(void* p_prevMenu);
extern void DeveloperMenuRun(void*);
extern void ConfigureDeveloperSettings();
extern void RemoteSetDefaultThresholdValues();
extern void SetTempInterventionType(InterventionType p_interventionType);
