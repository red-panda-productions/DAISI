/***************************************************************************

    file        : MyTrack.h
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

#ifndef _MYTRACK_H_
#define _MYTRACK_H_

#include "MyTrackSeg.h"
#include "Vec3d.h"

#include <track.h>
#include <vector>

class MyTrack
{
public:
  MyTrack();
  void   init(tTrack* track, double roughseglen);
  double yaw(double fromstart) const;
  double curvature(double fromstart) const;
  const  MyTrackSeg& seg(int idx) const;
  double yaw(int idx) const { return seg(idx).yaw; }
  int    nrOfSegs() const { return mTrack.size(); }
  double segLenMid() const { return mSegLenMid; }
  tTrack* torcsTrack() const { return mTorcsTrack; }
  char*  name() const { return mTorcsTrack->internalname; }
  double length() const { return mTorcsTrack->length; }
  double width() const { return mTorcsTrack->width; }

private:
  int    segIdx(double fromstart) const;
  void   calcPtAndNormal(tTrackSeg* seg, double toStart, Vec3d& pt, Vec3d& norm);

  tTrack* mTorcsTrack;
  std::vector<MyTrackSeg> mTrack;
  double mSegLenMid;
};

#endif // _MYTRACK_H_
