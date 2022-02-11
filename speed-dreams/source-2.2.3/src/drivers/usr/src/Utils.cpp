/***************************************************************************

    file        : Utils.cpp
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

#include "Utils.h"

#include <tgf.h>
#include <portability.h>


double Utils::distPtFromLine(Vec2d start, Vec2d end, Vec2d pnt)
{
    Vec2d line = end - start;
    line.normalize();
    Vec2d v = pnt - start;
    double distonline = v * line;
    Vec2d linepnt = start + line * distonline;

    return (pnt - linepnt).len();
}

bool Utils::LineCrossesLine(double p0x, double p0y, double v0x, double v0y, double p1x, double p1y, double v1x, double v1y, double& t)
{
    // double denom = lv0 % lv1;
    double denom = v0x * v1y - v0y * v1x;

    if (denom == 0)
    {
        return false;
    }

    // double numer = lv1 % (lp0 - lp1);
    double numer = v1x * (p0y - p1y) - v1y * (p0x - p1x);
    t = numer / denom;

    return true;
}

bool Utils::LineCrossesLineXY(const Vec3d& p0, const Vec3d& v0, const Vec3d& p1, const Vec3d& v1, double& t)
{
    return LineCrossesLine(p0.x, p0.y, v0.x, v0.y, p1.x, p1.y, v1.x, v1.y, t);
}

double Utils::CalcCurvature(double p1x, double p1y, double p2x, double p2y, double p3x, double p3y)
{
    double px = p1x - p2x;
    double py = p1y - p2y;
    double qx = p2x - p3x;
    double qy = p2y - p3y;
    double sx = p3x - p1x;
    double sy = p3y - p1y;
    double K = (2 * (px * qy - py * qx)) / sqrt((px * px + py * py) * (qx * qx + qy * qy) * (sx * sx + sy * sy));

    return K;
}

double Utils::CalcCurvatureXY(const Vec3d& p1, const Vec3d& p2, const Vec3d& p3)
{
    return CalcCurvature(p1.x, p1.y, p2.x, p2.y, p3.x, p3.y);
}

double Utils::CalcCurvatureZ(const Vec3d& p1, const Vec3d& p2, const Vec3d& p3)
{
    double x1 = 0;
    double x2 = (p1 - p2).len();
    double x3 = x2 + (p2 - p3).len();

    return CalcCurvature(x1, p1.z, x2, p2.z, x3, p3.z);
}

double Utils::normPiPi(double angle)
{
    while (angle > PI)
    {
        angle -= 2.0 * PI;
    }

    while (angle < -PI)
    {
        angle += 2.0 * PI;
    }

    return angle;
}

double Utils::calcCurvatureXY(const Vec3d& p1, const Vec3d& p2, const Vec3d& p3, const Vec3d& p4, const Vec3d& p5)
{
    double yawdiff1 = normPiPi(calcYaw(p2, p3, p4) - calcYaw(p1, p2, p3));
    double distdiff1 = (p3 - p2).len();
    double yawdiff2 = normPiPi(calcYaw(p3, p4, p5) - calcYaw(p2, p3, p4));
    double distdiff2 = (p4 - p3).len();

    return (yawdiff1 / distdiff1 + yawdiff2 / distdiff2) / 2.0;
}

double Utils::calcYaw(const Vec3d& p1, const Vec3d& p2, const Vec3d& p3)
{
    return (p3 - p1).getVec2().angle();
}

double Utils::calcRadius(double curvature)
{
    if (fabs(curvature) < 0.001)
    {
        return copysign(1.0, curvature) / 0.001;
    }
    else
    {
        return 1.0 / curvature;
    }
}

bool Utils::hysteresis(bool lastout, double in, double hyst)
{
    if (lastout == false)
    {
        if (in > hyst)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        if (in < -hyst)
        {
            return false;
        }
        else
        {
            return true;
        }
    }
}
