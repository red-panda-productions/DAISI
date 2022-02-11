/***************************************************************************

    file        : ParametricCubicSpline.h
    created     : 9 Apr 2006
    copyright   : (C) 2006 Tim Foden

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef _PARAMETRIC_CUBIC_SPLINE_H_
#define _PARAMETRIC_CUBIC_SPLINE_H_

#include "ParametricCubic.h"

#include <vector>

class ParametricCubicSpline
{
public:
    ParametricCubicSpline( int size, const Vec2d* points, const Vec2d* tangents );
    ~ParametricCubicSpline();

    bool CalcLineCrossingPt( const Vec2d& linePoint, const Vec2d& lineTangent, double* t );

private:
    std::vector<ParametricCubic> _curves;
};

#endif  // _PARAMETRIC_CUBIC_SPLINE_H_
