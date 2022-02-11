/***************************************************************************

    file        : Path.h
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

#ifndef _PATH_H_
#define _PATH_H_

#include "MyTrack.h"
#include "PathSeg.h"
#include "PathType.h"
#include "PathMargins.h"
#include "Vec3d.h"



class Path
{
public:
  Path(MyTrack* track, const std::string& datadir, const double clothfactor, const double vmaxk, const double vmaxkfactor, const PathType pathtype);
  PathType pathType() const { return mPathType; }
  double segLenMid() const { return mTrack->segLenMid(); }
  int nrOfSegs() const { return mTrack->nrOfSegs(); }
  double pathLength() const;
  const PathSeg& seg(int idx) const;
  int segIdx(double fromstart) const;
  double curvature(double fromstart) const;
  double toMiddle(double fromstart) const;
  double curvZ(double fromstart) const;
  double rollAngle(double fromstart) const;
  double yaw(double fromstart) const;
  double pathDistFromStart(double fromstart) const;
  double distOnPath(double fromstart1, double fromstart2) const;

private:
  void initPath();
  void generatePath();
  void optimisePath(int step);
  void optimise(double factor, PathSeg* l0, PathSeg* l1,PathSeg* l2, PathSeg* l3, PathSeg* l4, PathSeg* l5, PathSeg* l6);
  void smoothBetween(int step);
  void adjust(double kp, double lenp, double kn, double lenn, PathSeg* l, Vec3d pp, Vec3d pn);
  void setOffset(double t, PathSeg* l);
  double constrainOffset(double t, PathSeg* l);

  Vec3d calcPt(const PathSeg& seg);
  void calcCurvaturesXY();
  void calcCurvaturesZ();
  void calcSeglen();
  void calcPathDistFromStart();
  void calcDirection();
  void calcYawAndAngleToTrack();
  void calcPitchAngle();
  void calcRollAngle();
  void reverseToMidSign();

  PathMargins mMargins;
  std::vector<PathSeg> mPath;

  MyTrack* mTrack;
  PathType mPathType;
  double mClothFactor;
  double mVMaxK;
  double mVMaxKFactor;
  int mNSEG;
};


#endif // _PATH_H_
