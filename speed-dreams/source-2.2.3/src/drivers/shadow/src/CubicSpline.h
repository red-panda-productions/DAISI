/***************************************************************************

    file        : CubicSpline.h
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

// CubicSpline.h: interface for the CubicSpline class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _CUBICSPLINE_H_
#define _CUBICSPLINE_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Cubic.h"

class CubicSpline
{
public:
    CubicSpline( int n, const double* x, const double* y, const double* s );
    ~CubicSpline();

    double	CalcY( double x ) const;
    double	CalcGradient( double x ) const;

    bool	IsValidX( double x ) const;

private:
    int		FindSeg( double x ) const;

private:
    int		m_n;
    double*	m_pSegs;
    Cubic*	m_pCubics;
};

#endif // _CUBICSPLINE_H_
