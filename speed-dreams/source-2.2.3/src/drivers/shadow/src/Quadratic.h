/***************************************************************************

    file        : Quadratic.h
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

#ifndef _QUADRATIC_H_
#define _QUADRATIC_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class Quadratic
{
public:
    Quadratic();
    Quadratic( double a, double b, double c );
    Quadratic( double x, double y, double velY, double accY );
    ~Quadratic();

    void		Setup( double a, double b, double c );
    void		Setup( double x, double y, double velY, double accY );

    double		CalcMin() const;
    double		CalcY( double x ) const;
    bool		Solve( double y, double& x0, double& x1 ) const;
    bool		SmallestNonNegativeRoot( double& t ) const;

    Quadratic	operator+( const Quadratic& q ) const;
    Quadratic	operator-( const Quadratic& q ) const;

private:
    double		m_a;
    double		m_b;
    double		m_c;
};

#endif // _QUADRATIC_H_
