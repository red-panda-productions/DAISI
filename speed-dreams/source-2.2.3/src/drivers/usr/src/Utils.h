/***************************************************************************

    file        : Utils.h
    created     : 9 Apr 2006
    copyright   : (C) 2006 Tim Foden, 2019 D. Schellhammer

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef _UTILS_H_
#define _UTILS_H_

#include "Vec2d.h"
#include "Vec3d.h"

#include <algorithm>
#include <cmath>


namespace Utils
{
  bool LineCrossesLine(double p0x, double p0y, double v0x, double v0y, double p1x, double p1y, double v1x, double v1y, double& t);
  bool LineCrossesLineXY(const Vec3d& p0, const Vec3d& v0, const Vec3d& p1, const Vec3d& v1, double& t);
  double CalcCurvature(double p1x, double p1y, double p2x, double p2y, double p3x, double p3y);
  double CalcCurvatureXY(const Vec3d& p1, const Vec3d& p2, const Vec3d& p3);
  double CalcCurvatureZ(const Vec3d& p1, const Vec3d& p2, const Vec3d& p3);
  double normPiPi(double angle);

  double calcCurvatureXY(const Vec3d& p1, const Vec3d& p2, const Vec3d& p3, const Vec3d& p4, const Vec3d& p5); // smother than CalcCurvatureXY above
  double calcYaw(const Vec3d& p1, const Vec3d& p2, const Vec3d& p3); // yaw of middle point p2
  double calcRadius(double curvature);

  double distPtFromLine(Vec2d start, Vec2d end, Vec2d pnt);

  bool hysteresis(bool lastout, double in, double hyst);

  // Clip VALUE to the range LOW--HIGH.
  template <typename T> T clip(T value, T low, T high)
  {
    return std::max(std::min(value, high), low);
  }
}



#endif // _UTILS_H_
