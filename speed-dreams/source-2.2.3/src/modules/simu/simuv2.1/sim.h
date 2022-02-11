/***************************************************************************

    file                 : sim.h
    created              : Sun Mar 19 00:07:42 CET 2000
    copyright            : (C) 2000 by Eric Espie
    email                : torcs@free.fr
    version              : $Id: sim.h 3568 2011-05-15 15:55:24Z pouillot $

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef _SIMFCT_H_
#define _SIMFCT_H_

#include <cmath>
#include <cstdio>
#include <cstring>

#include <portability.h>
#include <tgf.h>
#include <track.h>
#include <car.h>
#include <raceman.h>
#include <robottools.h>

#include "carstruct.h"


extern void SimConfig(tCarElt *carElt);
extern void SimReConfig(tCarElt *carElt);
extern void SimUpdate(tSituation*, double deltaTime);
extern void SimInit(int nbcars, tTrack* track);
extern void SimShutdown(void);

extern void SimCarTelemetry(int nCarIndex, bool bOn = true);
extern void UpdateSimCarTable(tDynPt DynGCG,int index);
extern void SimUpdateSingleCar(int index, double deltaTime,tSituation *s);
extern tDynPt* GetSimCarTable(int index);


extern void SimAxleConfig(tCar *car, int index);
extern void SimAxleUpdate(tCar *car, int index);

extern void SimCarConfig(tCar *car);
extern void SimCarUpdate(tCar *car, tSituation*);
extern void SimCarUpdate2(tCar *car, tSituation*);

extern void SimSuspCheckIn(tSuspension *susp);
extern void SimSuspUpdate(tSuspension *susp);
extern void SimSuspConfig(void *hdle, const char *section, tSuspension *susp, tdble F0, tdble X0);

extern void SimWheelConfig(tCar *car, int index);
extern void SimWheelUpdateRide(tCar *car, int index);
extern void SimWheelUpdateForce(tCar *car, int index);
extern void SimWheelUpdateRotation(tCar *car);
extern void SimUpdateFreeWheels(tCar *car, int axlenb);


extern void SimSteerConfig(tCar *car);
extern void SimSteerUpdate(tCar *car);

extern void SimBrakeConfig(void *hdle, const char *section, tBrake *brake);
extern void SimBrakeUpdate(tCar *car, tWheel *wheel, tBrake *brake);
extern void SimBrakeSystemConfig(tCar *car);
extern void SimBrakeSystemUpdate(tCar *car);

extern void SimAeroConfig(tCar *car);
extern void SimAeroUpdate(tCar *car, tSituation *s);
extern void SimWingConfig(tCar *car, int index);
extern void SimWingUpdate(tCar *car, int index, tSituation *s);

extern void SimCarUpdateWheelPos(tCar *car);

extern void SimTransmissionConfig(tCar *car);
extern void SimTransmissionUpdate(tCar *car);
extern void SimGearboxUpdate(tCar *car);

extern void SimDifferentialConfig(void *hdle, const char *section, tDifferential *differential);
extern void SimDifferentialUpdate(tCar *car, tDifferential *differential, int first);

extern void SimEngineConfig(tCar *car);
extern void SimEngineUpdateTq(tCar *car);
extern tdble SimEngineUpdateRpm(tCar *car, tdble axleRpm);
extern void SimEngineShutdown(tCar *car);

extern void SimCarCollideZ(tCar *car);
extern void SimCarCollideXYScene(tCar *car);
extern void SimCarCollideCars(tSituation*);
extern void SimCarCollideConfig(tCar *car, tTrack *track);
extern void SimCarCollideShutdown(int nbcars);
extern void SimCarCollideInit(tTrack *track);
extern void SimCollideRemoveCar(tCar *car, int nbcars);

extern tdble SimDeltaTime;
extern int SimTelemetry;
extern tCar *SimCarTable;
extern t3Dd vectStart[];
extern t3Dd vectEnd[];

extern tdble simDammageFactor[];
extern tdble simSkidFactor[];

/// return a number drawn uniformly from [0,1]
inline float urandom() {
	return ((((float)rand()-1)/((float)RAND_MAX)));
}

#define SIM_VECT_COLL	12
#define SIM_VECT_SPD	13
#define SIM_VECT_ACCEL	14
#define SIM_WHEEL_SPD	16
#define SIM_WHEEL_ACCEL	20

#define SIM_Z_OFFSET	2.0

#endif /* _SIMFCT_H_ */ 



