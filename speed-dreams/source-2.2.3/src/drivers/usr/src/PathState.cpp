/***************************************************************************

    file        : PathState.cpp
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

#include "PathState.h"

#include "cubic.h"
#include "Utils.h"

// The "USR" logger instance.
extern GfLogger* PLogUSR;
#define LogUSR (*PLogUSR)

PathState::PathState(Path* path, MyCar* car, MuFactors* mufactors):
    mUpdatedLapData(false),
    mMaxspeed(0.0),
    mPath(path),
    mCar(car),
    mMuFactors(mufactors),
    mPathType(path->pathType()),
    mNSEG(path->nrOfSegs()),
    mSeedLimit(200.0)
{
    mData.clear();

    for (int i = 0; i < mNSEG; i++)
    {
        PathStateData s;
        s.speed = mSeedLimit;
        mData.push_back(s);
    }
}

void PathState::update(double dt)
{
    updateLapData();
    updateMaxspeedAcceleration(dt);
    updateOffset();
}

void PathState::updateLapData()
{
    bool letsupdate = false;

    if (segIdx(mCar->car()->_distFromStartLine) == 0)
    {
        mUpdatedLapData = false;
    }

    if (segIdx(mCar->car()->_distFromStartLine) >= 1)
    {
        letsupdate = true;
    }

    if (!mUpdatedLapData && letsupdate)
    {
        calcMaxSpeed();
        mUpdatedLapData = true;
    }
}

void PathState::updateMaxspeedAcceleration(double dt)
{
    double prevmaxspeed = mMaxspeed;
    double fs = mCar->car()->_distFromStartLine;
    mMaxspeed = 0.5 * (mCar->tires()->gripFactor() + sqrt(mCar->tires()->gripFactor())) * maxSpeed(fs);
    mAcceleration = (mMaxspeed - prevmaxspeed) / dt;
}

void PathState::updateOffset()
{
    mNearestPoint = nearestPointOnPath(mCar->mFrontAxleGlobalPos);
    mOffset = pathOffset(mNearestPoint, mCar->mFrontAxleGlobalPos);
}

void PathState::calcMaxSpeed()
{
    double speed;
    double nextspeed = mSeedLimit;
    double mass = mCar->mass();
    for (int i = 0; i < mNSEG; i++)
    {
        // Go through the path backwards
        int idx = mNSEG - 1 - i;
        PathSeg pathseg = mPath->seg(idx);
        PathSeg nextseg = mPath->seg(idx + 1);
        double seglen = pathseg.segLen;
        double mufactor = mMuFactors->muFactor(pathseg.fromStart);

        if (mPathType != PATH_O)
        {
            mufactor = mCar->muScaleLR() * mMuFactors->minMuFactor();
        }

        double mu = mufactor * mCar->tireMu() * pathseg.trackSeg->torcsSeg->surface->kFriction;
        double pitch = nextseg.pitchAngle;
        double roll = nextseg.rollAngle;
        double nextvv = nextspeed * nextspeed;
        //double brakeforce = mCar->brakeForce(nextspeed, nextseg.k, nextseg.kz, mCar->brakeMuFactor() * mu, pitch, roll, mPathType);
        double brakeforce = mCar->brakeForce(nextspeed, nextseg.k, nextseg.kz, mCar->brakeMuFactor() * mu, 0.0, roll, mPathType);
        double force = brakeforce + mCar->cw() * nextvv + pitch * mass * 9.81;
        double acc = -force / mass;

        // Calc the prev speed
        if (nextvv > 2 * acc * seglen)
        {
            speed = sqrt(nextvv - 2 * acc * seglen);
        }
        else
        {
            speed = nextspeed - acc * seglen / nextspeed;
        }
        // Keep the speed within the limits
        double cs = mCar->curveSpeed(pathseg.k, pathseg.kz, mu, roll, mPathType);
        double bs = mCar->bumpSpeed(pathseg.kz);
        cs = std::min(std::min(cs, bs), mSeedLimit);
        speed = std::min(speed, cs);
        mData[idx].speed = speed;
        nextspeed = speed;

        if (idx >= 0 && idx < mNSEG - 1)
        {
            LogUSR.debug("%d %g k:%g  cs:%g  v:%g  dif:%g  \n",idx, pathseg.fromStart, pathseg.k, 3.6*cs, 3.6*speed, 3.6 * (cs - speed));
        }
    }
}

double PathState::maxSpeed(double fromstart) const
{
    int idx = segIdx(fromstart);
    // Interpolate linear
    double poslen = fromstart - mPath->seg(idx).fromStart;

    return mData[idx].speed + (mData[(idx + 1) % mNSEG].speed - mData[idx].speed) * poslen / mPath->segLenMid();
}

PathSeg PathState::nearestPointOnPath(const Vec3d& pos) const
{
    Vec2d point;
    double distonline;
    int idx = nearestSegIndex(pos);

    if (!nearestPointOnFiniteLine(mPath->seg(idx).pt.getVec2(), mPath->seg(idx + 1).pt.getVec2(), pos.getVec2(), point, distonline))
    {
        idx = (idx - 1 + mNSEG) % mNSEG;
        nearestPointOnFiniteLine(mPath->seg(idx).pt.getVec2(), mPath->seg(idx + 1).pt.getVec2(), pos.getVec2(), point, distonline);
    }

    // Cubic interpolation
    Vec2d p0 = mPath->seg(idx).pt.getVec2();
    Vec2d p1 = mPath->seg(idx + 1).pt.getVec2();
    double s0 = Utils::normPiPi(mPath->seg(idx).yaw - (p1 - p0).angle());
    double s1 = Utils::normPiPi(mPath->seg(idx + 1).yaw - (p1 - p0).angle());
    Cubic cubic(0.0, 0.0, s0, mPath->seg(idx).segLen, 0.0, s1);
    double y = cubic.calcY(distonline);
    double ang = Utils::normPiPi(mPath->seg(idx).yaw + PI / 2);
    Vec2d norm = Vec2d(cos(ang), sin(ang));
    point += norm * y;

    PathSeg result;
    result.pt.x = point.x;
    result.pt.y = point.y;
    result.pt.z = 0.0;
    result.direction = mPath->seg(idx).direction;
    //result.fromStart = mRacePath->seg(idx).fromStart + distonline;
    result.k = mPath->seg(idx).k + (mPath->seg(idx + 1).k - mPath->seg(idx).k) * distonline / mPath->seg(idx).segLen;
    result.yaw = mPath->seg(idx).yaw + distonline * mPath->seg(idx).k; // interpolate linear

    return result;
}

int PathState::nearestSegIndex(const Vec3d& pos) const
{
    int idx = 0;
    double mindist = DBL_MAX;

    for (int i = 0; i < mNSEG; i++)
    {
        double dz = mPath->seg(i).pt.z - pos.z;

        if (fabs(dz) < 5.0)
        { // needed for crossing tracks
            double dist = ((mPath->seg(i).pt - pos).getVec2()).len();

            if (dist < mindist)
            {
                mindist = dist;
                idx = i;
            }
        }
    }

    return idx;
}

bool PathState::nearestPointOnFiniteLine(const Vec2d& start, const Vec2d& end, const Vec2d& pnt, Vec2d& res, double& distonline) const
{
    Vec2d line = end - start;
    double len = line.len();
    line.normalize();
    Vec2d v = pnt - start;
    distonline = Utils::clip(v * line, 0.0, len);
    res = start + line * distonline;

    if (distonline == 0.0 || distonline == len)
    {
        return false;
    }

    return true;
}

double PathState::pathOffset(const PathSeg& seg, const Vec3d& pos) const
{
    Vec2d pathdist = (seg.pt - pos).getVec2();
    // Which side of the path?
    Vec2d pathdir = seg.direction.getVec2();
    double cross = pathdir.fakeCrossProduct(&pathdist);
    int sign = 1;

    if (cross < 0.0)
    {
        sign = -1;
    }

    return sign * pathdist.len();
}

int PathState::segIdx(double fromstart) const
{
    return ((int)floor(fromstart / mPath->segLenMid()) + mNSEG) % mNSEG;
}
