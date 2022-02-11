/***************************************************************************

    file                 : pit.h
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

#ifndef _PIT_H_
#define _PIT_H_

#include <track.h>
#include <car.h>
#include <raceman.h>
#include <robottools.h>

#include "spline.h"
#include "MyCar.h"

#define PITPOINTS 7
#define DTPOINTS 4 // drive through

class Pit
{
public:
    Pit();
    void   init(const tTrack* track, const tSituation* situation, MyCar* car, int pitdamage, double pitgripfactor, double pitentrymargin);
    void   update();
    double dist();
    void   pitCommand();
    double pathToMiddle(double fromstart) const;
    bool   isBetween(double fromstart) const;
    bool   isPitLimit(double fromstart);
    bool   pitOnLeftSide() const { return mPitOnLeftSide; }
    double limitEntry() const { return mLimitEntry; }
    double speedLimit() const { return mSpeedLimit; }
    double avgFuelPerLap() const { return mAvgFuelPerLap; }
    bool   pitstop() const { return mPitstop; }
    int    penalty() const { return mPenalty; }

private:
    void   updateInPitLane(double fromstart);
    void   updateFuel(double fromstart);
    bool   pitBeforeTeammate(int remaininglaps) const;
    bool   pitForPenalty();
    void   setPitstop(bool pitstop);
    double toSplineCoord(double x) const;
    int    calcRepair() const;
    double calcRefuel();

    const tTrack* mTrack;
    const MyCar* mMyCar;
    tCarElt* mCar;
    const tCarElt* mTeamCar;
    const tTrackOwnPit* mPit; // pointer to my pit
    const tTrackPitInfo* mPitInfo; // general pit info
    bool mPitOnLeftSide;
    SplinePoint mPitp[PITPOINTS]; // pit spline points
    Spline mPitSpline;
    SplinePoint mDtp[DTPOINTS]; // drive through spline points
    Spline mDtSpline;
    int    mPenalty;
    bool   mPitstop;
    bool   mInPitLane; // we are still in the pit lane
    double mEntryMargin;
    double mPitEntry;
    double mPitExit;
    double mLimitEntry;
    double mLimitExit;
    double mSpeedLimit;
    bool   mFuelChecked;
    bool   mStopChecked;
    double mLastFuel;
    double mLastPitFuel;
    double mTotalFuel;
    int    mFuelLapsCounted;
    double mAvgFuelPerLap;
    bool   mTireChange;
    double mTiresChangeTime;
    int    mPitDamage;
    double mPitGripFactor;

    const int mMaxDamage;
    const int mMaxDamageDist;
    const double mPreEntryMargin;
};

#endif // _PIT_H_
