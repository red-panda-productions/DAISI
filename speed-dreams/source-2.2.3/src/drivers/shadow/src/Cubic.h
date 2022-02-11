/***************************************************************************

    file        : Cubic.h
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

// Cubic.h: interface for the Cubic class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _CUBIC_H_
#define _CUBIC_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class Cubic
{
public:
    Cubic();
    Cubic( double a, double b, double c, double d );
    Cubic(	double x0, double y0, double s0,
            double x1, double y1, double s1 );
    ~Cubic();

    void	Set( double a, double b, double c, double d );
    void	Set( double x0, double y0, double s0,
                 double x1, double y1, double s1 );

    double	CalcY( double x ) const;
    double	CalcGradient( double x ) const;
    double	Calc2ndDerivative( double x ) const;

    int		Solve( double* r1, double* r2, double* r3 ) const;

public:
    double	m_coeffs[4];	// coefficients
};

#endif // _CUBIC_H_
