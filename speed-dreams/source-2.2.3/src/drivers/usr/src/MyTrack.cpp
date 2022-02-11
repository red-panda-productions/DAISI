/***************************************************************************

    file        : MyTrack.cpp
    created     : 9 Apr 2006
    copyright   : (C) 2006 Tim Foden, 2013 D.Schellhammer

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "MyTrack.h"
#include "Utils.h"

#include <iostream>

MyTrack::MyTrack() :
    mTorcsTrack(NULL)
{
}

void MyTrack::init(tTrack* track, double roughseglen)
{
    if (mTorcsTrack != track)
    {
        mTrack.clear();
    }

    mTorcsTrack = track;

    if (mTrack.empty())
    {
        int NSEG = (int)floor(track->length / roughseglen);
        mSegLenMid = track->length / NSEG;
        tTrackSeg* torcsseg = track->seg;

        while (torcsseg->lgfromstart > 0.0)
        {
            torcsseg = torcsseg->next;
        }

        double tsend = torcsseg->lgfromstart + torcsseg->length;

        for (int i = 0; i < NSEG; i++)
        {
            double fromstart = i * mSegLenMid;

            while (fromstart >= tsend)
            {
                torcsseg = torcsseg->next;
                tsend = torcsseg->lgfromstart + torcsseg->length;
            }

            MyTrackSeg seg;
            seg.fromStart = fromstart;
            seg.torcsSeg = torcsseg;
            mTrack.push_back(seg);
        }

        // Calc point and normal
        for (int i = 0; i < NSEG; i++)
        {
            torcsseg = seg(i).torcsSeg;
            double fromstart = seg(i).fromStart;
            calcPtAndNormal(torcsseg, fromstart - torcsseg->lgfromstart, mTrack[i].midPnt, mTrack[i].normLR);
        }

        // Calc yaw and curvature
        for (int i = 0; i < NSEG; i++)
        {
            mTrack[i].yaw = Utils::normPiPi(mTrack[i].normLR.getVec2().angle() + PI / 2);
            mTrack[i].k = Utils::CalcCurvatureXY(seg(i-1).midPnt, seg(i).midPnt, seg(i+1).midPnt);
        }
    }

    // TODO: Bezier export
    //std::cout << "Bezier export:" << std::endl;
    std::vector<Vec3d>  controlPoint;
    std::vector<Vec3d>  direction;
    std::vector<double> fromstart;
    std::vector<double> radius;
    std::vector<double> arc;
    tTrackSeg* seg = track->seg;
    std::string segname = seg->name;
    double prevarc;
    double prevyaw = 0.0; // only init, first prevarc will be ignored
    double firstyaw = 0.0; // only init (compiler warning), will be assigned below

    for (int i = 0; i < track->nseg; i++)
    {
        if (seg->name != segname)
        {
            segname = seg->name;
            // Control point
            Vec3d cp = (Vec3d(seg->vertex[TR_SL].x, seg->vertex[TR_SL].y, seg->vertex[TR_SL].z) + Vec3d(seg->vertex[TR_SR].x, seg->vertex[TR_SR].y, seg->vertex[TR_SR].z)) / 2;
            // Yaw
            double yaw = Utils::normPiPi(seg->angle[TR_ZS]);
            // Pitch
            double pitch = (seg->angle[TR_YL] + seg->angle[TR_YR]) / 2;
            // Direction
            Vec3d dir = Vec3d(cos(yaw), sin(yaw), sin(pitch));
            dir.normalize();
            // Arc
            prevarc = Utils::normPiPi(fabs(yaw - prevyaw)); // doesn't work for arcs > PI
            prevyaw = yaw;
            // Store values
            controlPoint.push_back(cp);
            direction.push_back(dir);
            fromstart.push_back(seg->lgfromstart);
            radius.push_back(seg->radius);
            if (controlPoint.size() > 1) {
                arc.push_back(fabs(prevarc));
            }
            else
            {
                firstyaw = yaw;
            }
        }

        seg = seg->next;
    }

    // Last arc
    prevarc = Utils::normPiPi(fabs(firstyaw - prevyaw));
    arc.push_back(fabs(prevarc));
    // Close the loop
    controlPoint.push_back(controlPoint[0]);
    direction.push_back(direction[0]);
    fromstart.push_back(track->length);
    radius.push_back(radius[0]);
    arc.push_back(0);
    // Calc conrol point handles
    std::vector<Vec3d> prevHandle;
    std::vector<Vec3d> nextHandle;

    for (unsigned i = 0; i < controlPoint.size(); i++)
    {
        // Segment lengths
        double prevseglength = 0.0;

        if (i > 0)
        {
            prevseglength = fromstart[i] - fromstart[i - 1];
        }
        double nextseglength = 0.0;

        if (i < controlPoint.size() - 1)
        {
            nextseglength = fromstart[i + 1] - fromstart[i];
        }

        // Prev handle
        double prev_handle_len = 1.0;

        if (i > 0)
        {
            if (arc[i - 1] > 0.01)
            {
                prev_handle_len = radius[i - 1] * (4.0 / 3.0) * tan(arc[i - 1] / 4.0);
            }
            else
            {
                prev_handle_len = prevseglength / 4.0; // handle for straights
            }
        }

        Vec3d prev_handle = controlPoint[i] - direction[i] * prev_handle_len;
        prevHandle.push_back(prev_handle);
        // Next handle
        double next_handle_len = 1.0;

        if (i < controlPoint.size() - 1)
        {
            if (arc[i] > 0.01)
            {
                next_handle_len = radius[i] * (4.0 / 3.0) * tan(arc[i] / 4.0);
            }
            else
            {
                next_handle_len = nextseglength / 4.0; // handle for straights
            }
        }

        Vec3d next_handle = controlPoint[i] + direction[i] * next_handle_len;
        nextHandle.push_back(next_handle);
    }

    // Print
    for (unsigned i = 0; i < controlPoint.size(); i++)
    {
        //std::cout << prevHandle[i].x << " " << prevHandle[i].y << " " << prevHandle[i].z  << " "
        //          << controlPoint[i].x << " " << controlPoint[i].y << " " << controlPoint[i].z  << " "
        //          << nextHandle[i].x << " " << nextHandle[i].y << " " << nextHandle[i].z  << std::endl;
    }
}

double MyTrack::yaw(double fromstart) const
{
    int idx = segIdx(fromstart);
    // Interpolate linear
    double poslen = fromstart - seg(idx).fromStart;

    return Utils::normPiPi(seg(idx).yaw + Utils::normPiPi(seg(idx + 1).yaw - seg(idx).yaw) * poslen / mSegLenMid);
}

double MyTrack::curvature(double fromstart) const
{
    int idx = segIdx(fromstart);
    // Interpolate linear
    double poslen = fromstart - seg(idx).fromStart;

    return Utils::normPiPi(seg(idx).k + Utils::normPiPi(seg(idx + 1).k - seg(idx).k) * poslen / mSegLenMid);
}

const MyTrackSeg& MyTrack::seg(int idx) const
{
    return mTrack[(idx + nrOfSegs()) % nrOfSegs()];
}

int MyTrack::segIdx(double fromstart) const
{
    return ((int)floor(fromstart / mSegLenMid) + nrOfSegs()) % nrOfSegs();
}

void MyTrack::calcPtAndNormal(tTrackSeg* seg, double toStart, Vec3d& pt, Vec3d& norm)
{
    double t = toStart / seg->length;
    double hl = seg->vertex[TR_SL].z + (seg->vertex[TR_EL].z - seg->vertex[TR_SL].z) * t;
    double hr = seg->vertex[TR_SR].z + (seg->vertex[TR_ER].z - seg->vertex[TR_SR].z) * t;

    if (seg->type == TR_STR)
    {
        Vec3d s = (Vec3d(seg->vertex[TR_SL].x, seg->vertex[TR_SL].y, seg->vertex[TR_SL].z) + Vec3d(seg->vertex[TR_SR].x, seg->vertex[TR_SR].y, seg->vertex[TR_SR].z)) / 2;
        Vec3d e = (Vec3d(seg->vertex[TR_EL].x, seg->vertex[TR_EL].y, seg->vertex[TR_EL].z) + Vec3d(seg->vertex[TR_ER].x, seg->vertex[TR_ER].y, seg->vertex[TR_ER].z)) / 2;
        pt = s + (e - s) * t;
        norm = Vec3d(-seg->rgtSideNormal.x, -seg->rgtSideNormal.y, (hr - hl) / seg->width);
    }
    else
    {
        double d = seg->type == TR_LFT ? 1 : -1;
        double deltaAng = d * toStart / seg->radius;
        double ang = seg->angle[TR_ZS] - PI / 2 + deltaAng;
        double c = cos(ang);
        double s = sin(ang);
        double r = d * seg->radius;
        pt = Vec3d(seg->center.x + c * r, seg->center.y + s * r, (hl + hr) / 2);
        norm = Vec3d(c, s, (hr - hl) / seg->width);
    }
}
