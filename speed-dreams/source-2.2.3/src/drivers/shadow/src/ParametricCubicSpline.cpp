/***************************************************************************

    file        : ParametricCubicSpline.cpp
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

#include "ParametricCubicSpline.h"

ParametricCubicSpline::ParametricCubicSpline( int size, const Vec2d* points, const Vec2d* tangents )
{
    {for( int i = 0; i + 1 < size; i++ )
    {
//        _curves.push_back( ParametricCubic::FromPointsAndTangents(points[i],   tangents[i],
//                                                                  points[i+1], tangents[i+1]) );
        _curves.push_back( ParametricCubic::HaliteFromPointsAndTangents(
                                    points[i], tangents[i], points[i+1], tangents[i+1]) );
    }}
}

ParametricCubicSpline::~ParametricCubicSpline()
{
}

bool ParametricCubicSpline::CalcLineCrossingPt( const Vec2d& linePoint, const Vec2d& lineTangent, double* t )
{
    // slow method... try each curve in turn until a solution is found.
    {for( int i = 0; i < (int)_curves.size(); i++ )
    {
        if( _curves[i].Calc1stLineCrossingPt(linePoint, lineTangent, t) )
            return true;
    }}

    return false;
}
