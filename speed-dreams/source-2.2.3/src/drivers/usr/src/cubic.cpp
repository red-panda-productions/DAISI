/***************************************************************************

    file        : cubic.cpp
    created     : 18 Apr 2017
    copyright   : (C) 2017 Tim Foden, 2019 D.Schellhammer

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "cubic.h"

Cubic::Cubic(double x0, double y0, double s0, double x1, double y1, double s1)
{
    set(x0, y0, s0, x1, y1, s1);
}

void Cubic::set(double x0, double y0, double s0, double x1, double y1, double s1)
{
    // uses Ferguson's Parametric Cubic Curve, which requires 2
    // endpoints and 2 slopes.  here we define the endpoints to
    // to be (x0,y0) & (x1,y1), and the slopes are given by s0 & s1.
    //
    // see: http://graphics.cs.ucdavis.edu/CAGDNotes/
    // Catmull-Rom-Spline/Catmull-Rom-Spline.html
    // for the equations used.

    // step 1. convert to parametric form (x in [0..1])
    // (this basically only effects the slopes).
    double dx = x1 - x0;
    double dy = y1 - y0;
    s0 *= dx;
    s1 *= dx;

    // step 2. use Ferguson's method.
    double d = y0;
    double c = s0;
    double b = 3 * dy - 2 * s0 - s1;
    double a = -2 * dy + s0 + s1;

    // step 3. convert back to real-world form (x in [x0..x1]).
    double xx0  = x0  * x0;
    double xxx0 = xx0 * x0;
    double dxx  = dx  * dx;
    double dxxx = dxx * dx;
    mCoeffs[0] =      a        / dxxx;
    mCoeffs[1] = -3 * a * x0   / dxxx +     b       / dxx;
    mCoeffs[2] =  3 * a * xx0  / dxxx - 2 * b * x0  / dxx + c      / dx;
    mCoeffs[3] =     -a * xxx0 / dxxx +     b * xx0 / dxx - c * x0 / dx + d;
}

double Cubic::calcY(double x) const
{
    return ((mCoeffs[0] * x + mCoeffs[1]) * x + mCoeffs[2]) * x + mCoeffs[3];
}

double Cubic::calcGradient(double x) const
{
    return (3 * mCoeffs[0] * x + 2 * mCoeffs[1]) * x + mCoeffs[2];
}

double Cubic::calc2ndDerivative(double x) const
{
    return 6 * mCoeffs[0] * x + 2 * mCoeffs[1];
}
