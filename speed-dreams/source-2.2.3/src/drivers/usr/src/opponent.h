/***************************************************************************

    file                 : opponent.h
    created              : Thu Aug 31 01:21:49 UTC 2006
    copyright            : (C) 2006 Daniel Schellhammer

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef _OPPONENT_H_
#define _OPPONENT_H_

#include "MyCar.h"
#include "Path.h"

#include <track.h>
#include <car.h>

class Opponent
{
public:
  Opponent(tTrack* track, CarElt* car, MyCar* mycar, Path* mypath);
  void   update();
  bool   racing() const { return mRacing; };
  double fromStart() { return mOppCar->_distFromStartLine; };
  double v() const { return mSpeed; };
  double dist() const { return mDist; };
  bool   aside() const { return mAside; };
  double sideDist() const { return mSideDist; };
  double toMiddle() const { return mToMiddle; };
  double angle() const { return mAngle; };
  double distToStraight() const { return mDistToStraight; };
  bool   inDrivingDirection() const { return mInDrivingDirection; };
  double catchTime() const { return mCatchTime; };
  bool   behind() const { return mBehind; };
  bool   fastBehind() const { return mFastBehind; };
  double borderDist() const { return mBorderDist; };
  bool   teamMate() const { return mTeamMate; };
  bool   backMarker() const { return mBackMarker; };
  bool   letPass() const { return mLetpass; };
  bool   damaged() const { return mDamaged; };
  bool   leftOfMe() const { return mLeftOfMe; };

private:
  void   updateBasics();
  void   updateSpeed();
  double speedInYawDir(double yaw) const;
  void   updateDist();
  double cornerDist() const;
  double calcDistToStraight() const; // dist from front to back (x) line
  bool   calcBehind() const;
  double calcAngle() const; // angle relative to my car
  bool   calcInDrivingDirection() const;
  double calcCatchTime() const;
  bool   calcFastBehind() const;

  bool   mRacing;
  double mSpeed; // speed in path or my yaw direction
  double mDist; // approximation of the real distance
  bool   mAside;
  double mSideDist; // side distance of center of gravity of the cars
  double mToMiddle;
  double mAngle;
  double mDistToStraight;
  bool   mInDrivingDirection;
  double mCatchTime;
  bool   mBehind;
  bool   mFastBehind;
  double mBorderDist;
  bool   mBackMarker;
  bool   mLetpass;
  bool   mDamaged;
  bool   mLeftOfMe;

  CarElt* mOppCar; // pointer to the opponents car
  CarElt* mCar; // pointer to my car
  tTrack* mTrack;
  Path*   mMyPath; // ideal race path of my car
  bool    mTeamMate;
  double  mCarsDim; // dist between the cars centers when they are bumper to bumper
  double  mFrontRange;
  double  mBackRange;
};

#endif // _OPPONENT_H_
