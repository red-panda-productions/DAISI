/***************************************************************************

    file        : PathSeg.h
    created     : 11 Jan 2018
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


#ifndef _PATHSEG_H_
#define _PATHSEG_H_

#include "MyTrackSeg.h"

class PathSeg
{
public:
    const  MyTrackSeg* trackSeg; // track seg that contains this path seg
    double fromStart;
    double k; // curvature in xy
    double kz; // curvature in z direction... e.g. bumps
    double toMid; // offs from centre point
    Vec3d  pt; // actual point
    Vec3d  direction;
    double pitchAngle;
    double rollAngle; // roll in curve direction
    double yaw;
    double angleToTrack;
    double segLen;
    double pathDistFromStart;
};

#endif // _PATHSEG_H_
