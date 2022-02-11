/***************************************************************************

    file        : LinearRegression.cpp
    created     : 18 Apr 2017
    copyright   : (C) 2017 Tim Foden

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

// LinearRegression.cpp: implementation of the LinearRegression class.
//
//////////////////////////////////////////////////////////////////////

#include <robottools.h>

#include "LinearRegression.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

LinearRegression::LinearRegression()
:	m_n(0),
    m_sumX(0),
    m_sumY(0),
    m_sumXY(0),
    m_sumXX(0),
    m_sumYY(0)
{
}

LinearRegression::~LinearRegression()
{
}

void	LinearRegression::Clear()
{
    m_n = 0;
    m_sumX = 0;
    m_sumY = 0;
    m_sumXY = 0;
    m_sumXX = 0;
    m_sumYY = 0;
}

void	LinearRegression::Sample( double X, double Y )
{
    m_n++;
    m_sumX  += X;
    m_sumY  += Y;
    m_sumXY += X * Y;
    m_sumXX += X * X;
    m_sumYY += Y * Y;
}

void	LinearRegression::Sample( const Vec2d& p )
{
    Sample( p.x, p.y );
}

// vertical distance measure.
double	LinearRegression::CalcY( double X ) const
{
    double a, b;
    CalcCoeffs( &a, &b );
//	GfOut( "a %g   b %g\n", a, b );

    return a * X + b;
}

// perpendicular distance measure.
void	LinearRegression::CalcCoeffs( double* a, double* b ) const
{
    double	Xbar = m_sumX / m_n;
    double	Ybar = m_sumY / m_n;

//	*a = (m_sumXY + Xbar * m_sumY + m_sumX * Ybar + Xbar * Ybar) /
//		 (m_sumXX + Xbar * Xbar + 2 * m_sumX * Xbar);
    *a = (m_sumXY - Xbar * m_sumY - m_sumX * Ybar + Xbar * Ybar) /
         (m_sumXX + Xbar * Xbar - 2 * m_sumX * Xbar);

    *b = Ybar - (*a) * Xbar;
}

// perpendicular distance measure.
void	LinearRegression::CalcLine( Vec2d& p, Vec2d& v ) const
{
    p = Vec2d(m_sumX / m_n, m_sumY / m_n);

    // a = x - p.x, b = y - p.y
    double	sumAA = m_sumXX - 2 * p.x * m_sumX + m_n * p.x * p.x;
    double	sumBB = m_sumYY - 2 * p.y * m_sumY + m_n * p.y * p.y;
    double	sumAB = m_sumXY - p.y * m_sumX - p.x * m_sumY + m_n * p.x * p.y;

    double	ang = atan2(2 * sumAB, sumAA - sumBB) / 2;
    v = Vec2d(cos(ang), sin(ang));

//	DEBUGF( "x %g  y %g  a %g\n", p.x, p.y, ang );
}
