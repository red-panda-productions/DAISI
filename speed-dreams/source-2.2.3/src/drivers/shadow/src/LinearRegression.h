/***************************************************************************

    file        : LinearRegression.h
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

// LinearRegression.h: interface for the LinearRegression class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _LINEARREGRESSION_H_
#define _LINEARREGRESSION_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Vec2d.h"

class LinearRegression
{
public:
    LinearRegression();
    ~LinearRegression();

    void	Clear();
    void	Sample( double X, double Y );
    void	Sample( const Vec2d& p );
    double	CalcY( double X ) const;
    void	CalcCoeffs( double* a, double* b ) const;
    void	CalcLine( Vec2d& p, Vec2d& v ) const;

public:
    int		m_n;
    double	m_sumX;
    double	m_sumY;
    double	m_sumXY;
    double	m_sumXX;
    double	m_sumYY;
};

#endif // _LINEARREGRESSION_H_
