/***************************************************************************

    file        : cubic.h
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

#ifndef _CUBIC_H_
#define _CUBIC_H_

class Cubic
{
public:
    Cubic(double x0, double y0, double s0, double x1, double y1, double s1);

    double calcY(double x) const;
    double calcGradient(double x) const;
    double calc2ndDerivative(double x) const;

private:
    void set(double x0, double y0, double s0, double x1, double y1, double s1);

    double mCoeffs[4]; // coefficients
};

#endif // _CUBIC_H_
