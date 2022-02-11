/***************************************************************************

    file        : PathState.h
    created     : 19 Dec 2018
    copyright   : (C) 2018 D.Schellhammer

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef _PATHSTATE_H_
#define _PATHSTATE_H_

#include "Path.h"
#include "PathStateData.h"
#include "MyCar.h"
#include "MuFactors.h"
#include "Vec2d.h"

class PathState
{
public:
    PathState( Path* path,  MyCar* car,  MuFactors* mufactors);
    void   update(double dt);
    double maxSpeed(double fromstart) const;
    double maxSpeed() const { return mMaxspeed; }; // path maxspeed at this point of time
    double acceleration() const { return mAcceleration; }; // path acceleration at this point of time
    double offset() const { return mOffset; }; // path offset at this point of time
    double curvature() const { return mNearestPoint.k; }; // path curvature at this point of time
    double yaw() const { return mNearestPoint.yaw; }; // path yaw at this point of time

private:
    void    updateLapData(); // once a lap update of the path
    void    updateMaxspeedAcceleration(double dt);
    void    updateOffset();
    void    calcMaxSpeed();
    PathSeg nearestPointOnPath(const Vec3d& pos) const;
    int     nearestSegIndex(const Vec3d& pos) const;
    bool    nearestPointOnFiniteLine(const Vec2d& start, const Vec2d& end, const Vec2d& pnt, Vec2d& res, double& distonline) const;
    double  pathOffset(const PathSeg& seg, const Vec3d& pos) const;
    int     segIdx(double fromstart) const;

    std::vector<PathStateData> mData;
    bool mUpdatedLapData;
    double mMaxspeed;
    double mAcceleration;
    double mOffset;
    PathSeg mNearestPoint;

    Path* mPath;
    MyCar* mCar;
    MuFactors* mMuFactors;
    PathType mPathType;
    int mNSEG;
    double mSeedLimit;
};

#endif // _PATHSTATE_H_
