/***************************************************************************

    file        : MyTrackSeg.h
    created     : 07 Jan 2019
    copyright   : (C) 2019 D.Schellhammer

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef _MYTRACKSEG_H_
#define _MYTRACKSEG_H_

#include "Vec3d.h"

#include <track.h>

class MyTrackSeg
{
public:
  double fromStart;
  tTrackSeg* torcsSeg; // torcs track segment
  Vec3d  midPnt;       // middle point
  Vec3d  normLR;       // normal left to right (unit vector in xy, slope in z)
  double yaw;
  double k;            // curvature
};

#endif // _MYTRACKSEG_H_
