/***************************************************************************

    file        : MyCar.h
    created     : 8 Jun 2017
    copyright   : (C) 2017 D.Schellhammer

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef _MYCAR_H_
#define _MYCAR_H_

#include <string>
#include <car.h>

#include "filter.h"
#include "MyParam.h"
#include "MyTrack.h"
#include "PathType.h"
#include "pidcontroller.h"
#include "tires.h"
#include "Vec3d.h"

class MyCar
{
public:
  MyCar();

  void   init(tCarElt* car, MyTrack* track);
  void   readPrivateSection(const MyParam& param);
  void   readVarSpecs(const MyParam& param);
  void   readConstSpecs(void* CarHandle);
  double calcFuel(double dist) const;
  void   update(double dt);
  void   setControls(double accel, double brake, double steer);
  double brakeForce(double speed, double curvature, double curv_z, double mu, double pitchAngle, double rollAngle, PathType pathtype) const;
  double curveSpeed(double curvature, double curv_z, double mu, double rollAngle, PathType pathtype) const;
  double bumpSpeed(double curv_z) const;
  void   driveForward() { mGearDirection = 1; }
  void   driveBackwards() { mGearDirection = -1; }
  double v() const { return mSpeed; }
  double vX() const { return mSpeedX; }
  double accelFiltered() const { return mAccelFilter.get(); }
  double maxAccelForce() const { return mMaxAccelForce; }
  double maxBrakeForce() const { return mBrakeForceMax; }
  double cw() const { return mCW; }
  double segMu() const { return mSegMu; }
  double tireMu() const { return mTireMu; }
  double brakeMuFactor() const { return mBrakeMuFactor; }
  double muScaleLR() const { return mMuScaleLR; }
  double mass() const { return mMass; }
  double toMid() const { return mToMiddle; }
  double speedYaw() const { return mSpeedYaw; }
  double yaw() const { return mYaw; }
  double yawRate() const { return mYawRate; } // yawdiff / dist (the same as path curvature when driving slow)
  double angToTrack() const { return mAngleToTrack; }
  double borderDist() const { return mBorderDist; }
  double borderFriction() const { return mBorderFriction; }
  double wallDist() const { return mWalldist; }
  double wallToMiddleAbs() const { return mWallToMiddleAbs; }
  bool   pointingToWall() const { return mPointingToWall; }
  int    damageDiff() const { return mDamageDiff; }
  double sideSlip() const { return mSideSlip; }
  double FuelperMeter() const { return mFuelPerMeter; }
  bool   learningOfftrack();
  double filterABS(double brake);
  double filterTCL(double accel);
  double slipFront() const;
  double slipRear() const;
  double filterTCLSideSlip(double accel) const ;
  tCarElt* car()const { return mCar; }
  const Tires* tires() const { return &mTires; }
  int    gear() const { return mCar->_gear; }

  Vec3d mFrontAxleGlobalPos;
  double mSpeed;    // COG speed
  double mSpeedX;   // speed in cars yaw direction
  double mSpeedYaw; // yaw of COG speed vector
  double mSlip;
  double mSideSlip;
  double mToMiddle;

  bool    HASTYC;
  bool    HASABS;
  bool    HASESP;
  bool    HASTCL;

private:
  void   initVars();
  void   initCa();
  void   initCw();
  void   initBrakes();
  int    calcGear();
  double calcClutch();

  double mDeltaTime;
  tCarElt* mCar;
  const MyTrack* mTrack;
  Tires mTires;
  Vec3d mGlobalPos;
  double mFrontAxleOffset;
  int mGearDirection;
  int mShiftTimer;
  int mPrevGear;
  double mClutch;
  double mAbsFactor;
  double mTclFactor;
  PidController mTclController;
  double mAccel;
  Filter mAccelFilter;
  double mMaxAccelForce;
  double mSegMu;
  double mMass;
  Vec3d mHeading;
  double mYaw;
  double mYawRate;
  double mAngleToTrack;
  double mBorderDist;
  double mBorderFriction;
  int mDamageDiff;
  int mLastDamage;
  double mWalldist;
  double mWallToMiddleAbs;
  bool mPointingToWall;

  // Private configuration file constants
  double mAbsSlip;
  double mBrakeMuFactor;
  double mMuScaleLR;
  double mBumpSpeedFactor;
  double mFuelPerMeter;
  double mFuelWeightFactor;
  double mTireWearPerMeter;

  // Configuration file constants
  double mBrakePressMax;
  double mBrakeRepartition;
  double mFrontWingAngle;
  double mRearWingAngle;
  double mSideSlipTCL;
  double mSideSlipTCLQualy;
  double mSideSlipTCLFactor;

  // Constants
  double mCarMass;
  double mTankVol;
  double mTireMu;

  // Aerodynamic constants
  double mFrontCA;
  double mRearCA;
  double mCA; // aerodynamic downforce coefficient
  double mBodyCW; // body aerodynamic drag coefficient
  double mWingCW; // wing aerodynamic drag coefficient
  double mCW;

  // Brake constant
  double mBrakeForceMax;
};

#endif // _MYCAR_H_
