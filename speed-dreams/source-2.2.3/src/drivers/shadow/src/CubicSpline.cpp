/***************************************************************************

    file        : CubicSpline.cpp
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

// CubicSpline.cpp: implementation of the CubicSpline class.
//
//////////////////////////////////////////////////////////////////////

#include "CubicSpline.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CubicSpline::CubicSpline( int n, const double* x, const double* y, const double* s )
{
	m_n = n;
	m_pSegs = new double[n];
	m_pCubics = new Cubic[n - 1];

	for( int i = 0; i < n; i++ )
	{
		m_pSegs[i] = x[i];
		if( i + 1 < n )
			m_pCubics[i].Set( x[i], y[i], s[i], x[i + 1], y[i + 1], s[i + 1] );
	}
}

CubicSpline::~CubicSpline()
{
	delete [] m_pSegs;
	delete [] m_pCubics;
}

double	CubicSpline::CalcY( double x ) const
{
	int	i = FindSeg(x);
	return m_pCubics[i].CalcY(x);
}

double	CubicSpline::CalcGradient( double x ) const
{
	int	i = FindSeg(x);
	return m_pCubics[i].CalcGradient(x);
}

bool	CubicSpline::IsValidX( double x ) const
{
	return x >= m_pSegs[0] && x <= m_pSegs[m_n - 1];
}

int	CubicSpline::FindSeg( double x ) const
{
	// binary chop search for interval.
	int	lo = 0;
	int	hi = m_n;

	while( lo + 1 < hi )
	{
		int mid = (lo + hi) / 2;
		if( x >= m_pSegs[mid] )
			lo = mid;
		else
			hi = mid;
	}

	return lo;
}
