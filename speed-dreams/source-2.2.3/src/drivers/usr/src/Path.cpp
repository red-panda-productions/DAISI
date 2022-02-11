/***************************************************************************

    file        : Path.cpp
    created     : 9 Apr 2006
    copyright   : (C) 2006 Tim Foden, 2017 W.D. Beelitz, 2019 D.Schellhammer

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "Path.h"
#include "Utils.h"

#include <iostream>
#include <portability.h>

Path::Path(MyTrack* track, const std::string& datadir, const double clothfactor, const double vmaxk, const double vmaxkfactor, const PathType pathtype) :
    mTrack(track),
    mPathType(pathtype),
    mClothFactor(clothfactor),
    mVMaxK(vmaxk),
    mVMaxKFactor(vmaxkfactor),
    mNSEG(track->nrOfSegs())
{
    // File with path margins
    std::string marginsfile = datadir + track->name() + "_margins.dat";
    mMargins.read(marginsfile);

    initPath();
    generatePath();
    calcCurvaturesXY();
    calcCurvaturesZ();
    calcSeglen();
    calcPathDistFromStart();
    calcDirection();
    calcYawAndAngleToTrack();
    calcPitchAngle();
    calcRollAngle();
    reverseToMidSign();

    if (pathtype == PATH_O)
    {
        for (int i = 0; i < mNSEG; i++)
        {
            //      std::cout << mPath[i].pt.x << " " << mPath[i].pt.y << std::endl;
            //      std::cout << mPath[i].fromStart << " " << 1000*mPath[i].k << " " << mPath[i].yaw << std::endl;
        }
    }
}

void Path::initPath()
{
    mPath.clear();

    for (int i = 0; i < mNSEG; i++)
    {
        PathSeg seg;
        seg.trackSeg = &(mTrack->seg(i));
        seg.fromStart = seg.trackSeg->fromStart;
        seg.k = 0.0;
        seg.kz = 0.0;
        seg.toMid = 0.0;
        seg.pt = calcPt(seg);
        mPath.push_back(seg);
    }
}

void Path::generatePath()
{
    // Number of iterations in the inner loop
    int iter = 25;

    // Nr of outer loops per step
    int loops = (150 + iter - 1) / iter;

    // Find largest step width usable
    int  step = 1;

    while (step * 8 < mNSEG)
    {
        step *= 2;
    }

    // Start the recursive algorithm
    while (step > 0) {
        for (int i = 0; i < loops; i++)
        {
            for (int j = 0; j < iter; j++)
            {
                optimisePath(step);
            }

            // Now smooth the values between steps
            if (step > 1)
            {
                smoothBetween(step);
            }
        }

        step >>= 1; // divide the step witdh by 2
    }

    // Final optimization, we need only one step and no loop
    step = 1;

    for (int j = 0; j < iter; j++)
    {
        optimisePath(step);
    }
}

void Path::optimisePath(int step)
{
    PathSeg* l0 = NULL;
    PathSeg* l1 = &mPath[mNSEG - 3 * step];
    PathSeg* l2 = &mPath[mNSEG - 2 * step];
    PathSeg* l3 = &mPath[mNSEG - step];
    PathSeg* l4 = &mPath[0];
    PathSeg* l5 = &mPath[step];
    PathSeg* l6 = &mPath[2 * step];

    // Go forwards
    int i = 3 * step;
    int n = (mNSEG + step - 1) / step;

    for (int count = 0; count < n; count++)
    {
        l0 = l1;
        l1 = l2;
        l2 = l3;
        l3 = l4;
        l4 = l5;
        l5 = l6;
        l6 = &mPath[i];
        optimise(mClothFactor, l0, l1, l2, l3, l4, l5, l6);
        i = (i + step) % mNSEG;
    }
}

void Path::optimise(double factor, PathSeg* l0, PathSeg* l1, PathSeg* l2, PathSeg* l3, PathSeg* l4, PathSeg* l5, PathSeg* l6)
{
    Vec3d p0 = l0->pt;
    Vec3d p1 = l1->pt;
    Vec3d p2 = l2->pt;
    Vec3d p3 = l3->pt;
    Vec3d p4 = l4->pt;
    Vec3d p5 = l5->pt;
    Vec3d p6 = l6->pt;

    double k1 = Utils::CalcCurvatureXY(p0, p1, p2);
    double k2 = Utils::CalcCurvatureXY(p1, p2, p3);
    double k4 = Utils::CalcCurvatureXY(p3, p4, p5);
    double k5 = Utils::CalcCurvatureXY(p4, p5, p6);

    if (k2 * k4 > 0)
    {
        if (k1 * k2 > 0 && k4 * k5 > 0)
        {
            if (fabs(k1) < fabs(k2) && fabs(k2) * 1.02 < fabs(k4))
            {
                k2 *= factor;
            }
            else if (fabs(k1) > fabs(k2) * 1.02 && fabs(k2) > fabs(k4))
            {
                k2 *= factor;
            }
        }
    }
    else if (k2 * k4 < 0)
    {
        if (k1 * k2 > 0 && k4 * k5 > 0)
        {
            if (fabs(k2) < fabs(k4) && fabs(k2) < fabs(k5))
            {
                k2 = k2 * 0.25 + k4 * 0.75;
            } else if (fabs(k4) < fabs(k2) && fabs(k4) < fabs(k1))
            {
                k4 = k4 * 0.25 + k2 * 0.75;
            }
        }
    }

    double len2 = hypot(p3.x - p2.x, p3.y - p2.y);
    double len4 = hypot(p4.x - p3.x, p4.y - p3.y);
    adjust(k2, len2, k4, len4, l3, p2, p4);
}

void Path::smoothBetween(int step)
{
    PathSeg* l0 = nullptr;
    PathSeg* l1 = &mPath[mNSEG - step];
    PathSeg* l2 = &mPath[0];
    PathSeg* l3 = &mPath[step];

    int j = 2 * step;

    for (int i = 0; i < mNSEG; i += step)
    {
        l0 = l1;
        l1 = l2; // l1 represents mPath[i];
        l2 = l3;
        l3 = &mPath[j];
        j = (j + step) % mNSEG;

        Vec3d p0 = l0->pt;
        Vec3d p1 = l1->pt;
        Vec3d p2 = l2->pt;
        Vec3d p3 = l3->pt;

        double k1 = Utils::CalcCurvatureXY(p0, p1, p2);
        double k2 = Utils::CalcCurvatureXY(p1, p2, p3);

        for (int k = 1; k < step; k++)
        {
            PathSeg* l = &(mPath[(i + k) % mNSEG]);
            double len1 = (calcPt(*l) - p1).len();
            double len2 = (calcPt(*l) - p2).len();
            adjust(k1, len1, k2, len2, l, p1, p2);
        }
    }
}

void Path::adjust(double kp, double lenp, double kn, double lenn, PathSeg* l, Vec3d pp, Vec3d pn)
{
    double kappa = (kp * lenn + kn * lenp) / (lenp + lenn);

    //double vmaxk = 0.0018; // vmaxk = G * tyreMu / (MAX_SPD * MAX_SPD)
    if ((kp * kn >= 0) && (fabs(kp) < mVMaxK) && (fabs(kn) < mVMaxK))
    {
        kappa *= mVMaxKFactor;// 0.9;
    }

    double t;
    Utils::LineCrossesLineXY(l->trackSeg->midPnt, l->trackSeg->normLR, pp, pn - pp, t);
    double delta = 0.0001;
    double deltak = Utils::CalcCurvatureXY(pp, l->trackSeg->midPnt + l->trackSeg->normLR * (t + delta), pn);
    t += delta * kappa / deltak;
    setOffset(t, l);
}

void Path::setOffset(double t, PathSeg* l)
{
    t = constrainOffset(t, l);
    l->toMid = t;
    l->pt = calcPt(*l);
}

double Path::constrainOffset(double t, PathSeg* l)
{
    PathMarginsSect marg = mMargins.margins(l->fromStart);
    double maxL = mTrack->width() / 2.0 - marg.left;
    double maxR = mTrack->width() / 2.0 - marg.right;

    if (mPathType == PATH_L)
    {
        maxR = -1.0;
    }
    else if (mPathType == PATH_R)
    {
        maxL = -1.0;
    }

    return Utils::clip(t, -maxL, maxR);
}

double Path::pathLength() const
{
    return seg(mNSEG - 1).pathDistFromStart + seg(mNSEG - 1).segLen;
}

const PathSeg& Path::seg(int idx) const
{
    return mPath[(idx + mNSEG) % mNSEG];
}

int Path::segIdx(double fromstart) const
{
    return ((int)floor(fromstart / segLenMid()) + mNSEG) % mNSEG;
}

void Path::calcCurvaturesXY()
{
    for (int i = 0; i < mNSEG; i++)
    {
        mPath[i].k = Utils::calcCurvatureXY(seg(i - 2).pt, seg(i - 1).pt, seg(i).pt, seg(i + 1).pt, seg(i + 2).pt);
    }
}

void Path::calcCurvaturesZ()
{
    for (int count = 0; count < mNSEG; count++)
    {
        int i = (count) % mNSEG;
        int ip = (i - 1 + mNSEG) % mNSEG;
        int in = (i + 1) % mNSEG;
        mPath[i].kz = Utils::CalcCurvatureZ(mPath[ip].pt, mPath[i].pt, mPath[in].pt);
    }

    // Filter
    for (int count = 0; count < mNSEG; count++)
    {
        int i = (count) % mNSEG;
        int ip = (i - 1 + mNSEG) % mNSEG;
        int in = (i + 1) % mNSEG;
        mPath[ip].kz = (mPath[i].kz + mPath[in].kz) / 2.0;
    }
}

void Path::calcSeglen()
{
    for (int i = 0; i < mNSEG; i++)
    {
        mPath[i].segLen = (seg(i + 1).pt - seg(i).pt).len();
    }
}

void Path::calcPathDistFromStart()
{
    mPath[0].pathDistFromStart = 0.0;

    for (int i = 1; i < mNSEG; i++)
    {
        mPath[i].pathDistFromStart = seg(i - 1).pathDistFromStart + seg(i - 1).segLen;
    }
}

void Path::calcDirection()
{
    for (int i = 0; i < mNSEG; i++)
    {
        if ( i < (int)mPath.size() - 1 )
        {
            mPath[i].direction = seg(i + 1).pt - seg(i).pt;
        }
        else
        {
            mPath[i].direction = seg(1).pt - seg(i - 1).pt;
        }

        mPath[i].direction.normalize();
    }
}

void Path::calcYawAndAngleToTrack()
{
    for (int i = 0; i < mNSEG; i++)
    {
        mPath[i].yaw = Utils::calcYaw(seg(i - 1).pt, seg(i).pt, seg(i + 1).pt);
        mPath[i].angleToTrack = Utils::normPiPi(mPath[i].yaw - mTrack->yaw(i));
    }
}

void Path::calcPitchAngle()
{
    for (int i = 0; i < mNSEG; i++)
    {
        mPath[i].pitchAngle = asin((seg(i + 1).pt.z - seg(i).pt.z) / mPath[i].segLen);
    }
}

void Path::calcRollAngle()
{
    for (int i = 0; i < mNSEG; i++)
    {
        mPath[i].rollAngle = atan2(seg(i).trackSeg->normLR.z, 1) * copysign(1.0, mPath[i].k);
    }
}

void Path::reverseToMidSign()
{
    for (int i = 0; i < mNSEG; i++)
    {
        mPath[i].toMid = -mPath[i].toMid;
    }
}

Vec3d Path::calcPt(const PathSeg& seg)
{
    return seg.trackSeg->midPnt + seg.trackSeg->normLR * seg.toMid;
}

double Path::curvature(double fromstart) const
{
    int idx = segIdx(fromstart);
    // Interpolate linear
    double poslen = fromstart - seg(idx).fromStart;

    return seg(idx).k + (seg(idx + 1).k - seg(idx).k) * poslen / segLenMid();
}

double Path::toMiddle(double fromstart) const
{
    int idx = segIdx(fromstart);
    // Interpolate linear
    double poslen = fromstart - seg(idx).fromStart;

    return seg(idx).toMid + (seg(idx + 1).toMid - seg(idx).toMid) * poslen / segLenMid();
}

double Path::curvZ(double fromstart) const
{
    return seg(segIdx(fromstart)).kz;
}

double Path::rollAngle(double fromstart) const
{
    return seg(segIdx(fromstart)).rollAngle;
}

double Path::yaw(double fromstart) const
{
    int idx = segIdx(fromstart);
    // Interpolate linear
    double poslen = fromstart - seg(idx).fromStart;

    return Utils::normPiPi(seg(idx).yaw + seg(idx).k * poslen / segLenMid());
}

double Path::pathDistFromStart(double fromstart) const
{
    int idx = segIdx(fromstart);
    // Interpolate linear
    double poslen = fromstart - seg(idx).fromStart;

    return seg(idx).pathDistFromStart + seg(idx).segLen * poslen / segLenMid();
}

double Path::distOnPath(double fromstart1, double fromstart2) const
{
    double dist = pathDistFromStart(fromstart2) - pathDistFromStart(fromstart1);

    if (dist > pathLength() / 2.0)
    {
        dist -= pathLength();
    }
    else if (dist < -pathLength() / 2.0)
    {
        dist += pathLength();
    }

    return dist;
}
