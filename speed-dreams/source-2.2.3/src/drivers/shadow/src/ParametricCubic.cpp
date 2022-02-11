/***************************************************************************

    file        : ParametricCubic.cpp
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

#include "ParametricCubic.h"
#include "Utils.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ParametricCubic::ParametricCubic()
{
}

ParametricCubic::~ParametricCubic()
{
}

void	ParametricCubic::SetPoints( Vec2d p0, Vec2d p1, Vec2d p2, Vec2d p3 )
{
    Vec2d	v0, v1;
    Utils::CalcTangent( p0, p1, p2, v0 );
    Utils::CalcTangent( p1, p2, p3, v1 );
    double	len = (p2 - p1).len();// * 1.15;

    v0 = v0 * len;
    v1 = v1 * len;
    m_x.Set( 0, p1.x, v0.x, 1, p2.x, v1.x );
    m_y.Set( 0, p1.y, v0.y, 1, p2.y, v1.y );

//	SetHalitePointsAndTangents( p0, v0, p1, v1 );
}

void	ParametricCubic::SetPointsAndTangents( Vec2d p0, Vec2d v0, Vec2d p1, Vec2d v1 )
{
    //double	v0Len = 1;//v0.len();
    //double	v1Len = 1;//v1.len();
    //v0Len = v1Len = 1.0 / (p1 - p0).len();
    //m_x.Set( 0, p0.x, v0.x / v0Len, 1, p1.x, v1.x / v1Len );
    //m_y.Set( 0, p0.y, v0.y / v0Len, 1, p1.y, v1.y / v1Len );
    double dx = p1.x - p0.x;
    double dy = p1.y - p0.y;
    m_x.Set( 0, p0.x, v0.x * dx, 1, p1.x, v1.x * dx );
    m_y.Set( 0, p0.y, v0.y * dy, 1, p1.y, v1.y * dy );
}

// For posterity I think that the Halite here was a misnomer, and should have been Hermite.
// from: https://www.cs.helsinki.fi/group/goa/mallinnus/curves/curves.html
// it can be seen that Hermite curves have a basis matrix of:
//
//          p0 p1 v0 v1
//	      [  2 -2  1  1 ]
//	M_h = [ -3  3 -2 -1 ]
//	      [  0  0  1  0 ]
//	      [  1  0  0  0 ]
//
// ... which seems to correspond exactly to the values used in the function below.
void	ParametricCubic::SetHalitePointsAndTangents( Vec2d p0, Vec2d v0, Vec2d p1, Vec2d v1 )
{
    double scale = hypot(p1.x - p0.x, p1.y - p0.y);

    m_x.Set( 2 * p0.x +     v0.x * scale - 2 * p1.x + v1.x * scale,
            -3 * p0.x - 2 * v0.x * scale + 3 * p1.x - v1.x * scale,
            v0.x * scale, p0.x);

    m_y.Set( 2 * p0.y +     v0.y * scale - 2 * p1.y + v1.y * scale,
            -3 * p0.y - 2 * v0.y * scale + 3 * p1.y - v1.y * scale,
            v0.y * scale, p0.y);
}

ParametricCubic ParametricCubic::FromPoints( Vec2d p0, Vec2d p1, Vec2d p2, Vec2d p3 )
{
    ParametricCubic cubic;
    cubic.SetPoints( p0, p1, p2, p3 );
    return cubic;
}

ParametricCubic ParametricCubic::FromPointsAndTangents( Vec2d p0, Vec2d v0, Vec2d p1, Vec2d v1 )
{
    ParametricCubic cubic;
    cubic.SetPointsAndTangents( p0, v0, p1, v1 );
    return cubic;
}

ParametricCubic ParametricCubic::HaliteFromPointsAndTangents( Vec2d p0, Vec2d v0, Vec2d p1, Vec2d v1 )
{
    ParametricCubic cubic;
    cubic.SetHalitePointsAndTangents( p0, v0, p1, v1 );
    return cubic;
}

Cubic&	ParametricCubic::GetCubic( int which )
{
    return which == 0 ? m_x : m_y;
}

Vec2d	ParametricCubic::Calc( double t ) const
{
    double	x = m_x.CalcY(t);
    double	y = m_y.CalcY(t);
    return Vec2d(x, y);
}

Vec2d	ParametricCubic::CalcGradient( double t ) const
{
    double	dx = m_x.CalcGradient(t);
    double	dy = m_y.CalcGradient(t);
    return Vec2d(dx, dy);
}

double	ParametricCubic::CalcCurvature( double t ) const
{
    // signed curvature....
    //
    //          x'y" - y'x"
    //	K = -------------------
    //      (x'^2 + y'^2)^(3/2)

    double	x1d = m_x.CalcGradient(t);
    double	x2d = m_x.Calc2ndDerivative(t);
    double	y1d = m_y.CalcGradient(t);
    double	y2d = m_y.Calc2ndDerivative(t);

    double	k = (x1d * y2d - y1d * x2d) / pow(x1d * x1d + y1d * y1d, 3.0 / 2);

//	Vec2d	p0 = Calc(0);
//	Vec2d	pp = Calc(t);
//	Vec2d	p1 = Calc(1);
//	double	k2 = Utils::CalcCurvature(p0, pp, p1);

    return k;
}
bool    ParametricCubic::Calc1stLineCrossingPt( const Vec2d& linePt, const Vec2d& lineTan, double* t ) const
{
    double	lineDX = lineTan.x;
    double	lineDY = lineTan.y;

    double	A, B, C, D;

    if( lineDX == 0.0 )
    {
        // line is constant in y --> solve for x only
        A = m_x.m_coeffs[0];
        B = m_x.m_coeffs[1];
        C = m_x.m_coeffs[2];
        D = m_x.m_coeffs[3] - linePt.x;
    }
    else if( lineDY == 0.0 )
    {
        // line is constant in x --> solve for y only
        A = m_y.m_coeffs[0];
        B = m_y.m_coeffs[1];
        C = m_y.m_coeffs[2];
        D = m_y.m_coeffs[3] - linePt.y;
    }
    else
    {
        // line varies in both x and y
        double	scale = lineDY / lineDX;
        A = m_x.m_coeffs[0] * scale - m_y.m_coeffs[0];
        B = m_x.m_coeffs[1] * scale - m_y.m_coeffs[1];
        C = m_x.m_coeffs[2] * scale - m_y.m_coeffs[2];
        D = (m_x.m_coeffs[3] - linePt.x) * scale - (m_y.m_coeffs[3] - linePt.y);
    }

    double roots[3];
    int nRoots = Utils::SolveCubic(A, B, C, D, &roots[0], &roots[1], &roots[2]);
    if( nRoots == 0 )
        return false;

    int best = -1;
    double bestT = 0;
    for( int i = 0; i < nRoots; i++ )
    {
        if( roots[i] < -0.001 || roots[i] > 1.001 )
        {
            // not within curve.
            continue;
        }

        Vec2d curvePt = Calc(roots[i]);
        double u = Utils::ClosestPtOnLine(curvePt.x, curvePt.y, linePt.x, linePt.y, lineTan.x, lineTan.y);
        if( best < 0 || fabs(bestT) > fabs(u) )
        {
            best  = i;
            bestT = u;
        }
    }

    if( best < 0 )
        return false;

    if( t ) *t = bestT;
    return true;
}
