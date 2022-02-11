/***************************************************************************

    file        : Quadratic.cpp
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

#include "Quadratic.h"

#include <math.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Quadratic::Quadratic()
:	m_a(0),
    m_b(0),
    m_c(0)
{
}

Quadratic::Quadratic( double a, double b, double c )
:	m_a(a),
    m_b(b),
    m_c(c)
{
}

Quadratic::Quadratic( double x, double y, double velY, double accY )
{
    Setup( x, y, velY, accY );
}

Quadratic::~Quadratic()
{
}

void	Quadratic::Setup( double a, double b, double c )
{
    m_a = a;
    m_b = b;
    m_c = c;
}

void	Quadratic::Setup( double x, double y, double velY, double accY )
{
    m_a = accY / 2;
    m_b = velY - 2 * m_a * x;
    m_c = y - (m_a * x + m_b) * x;
}

double	Quadratic::CalcMin() const
{
    // minimum is where slope == 0
    double	x = -m_b / (2 * m_a);
    return x;
}

double	Quadratic::CalcY( double x ) const
{
    return (m_a * x + m_b) * x + m_c;
}

bool	Quadratic::Solve( double y, double& x0, double& x1 ) const
{
    if( m_a == 0 )
    {
        if( m_b == 0 )
        {
            return false;
        }

        // y == bx + c
        //
        // x = (y - c) / b

        x0 = x1 = (y - m_c) / m_b;
        return true;
    }

    // y == a * x * x + b * x + c
    //
    // a * x * x + b * x + (c - y) == 0
    //
    // x = (-b +/- sqrt(b * b - 4 * a * (c - y))] / (2 * a)

    double	inner = m_b * m_b - 4 * m_a * (m_c - y);
    if( inner < 0 )
        return false;

    inner = sqrt(inner);
    x0 = (-m_b - inner) / (2 * m_a);
    x1 = (-m_b + inner) / (2 * m_a);

    return true;
}

bool	Quadratic::SmallestNonNegativeRoot( double& t ) const
{
    double	x0, x1;
    if( !Solve(0, x0, x1) )
        return false;

    t = x0;
    if( x1 >= 0 && x1 < x0 )
        t = x1;

    return t >= 0;
}

Quadratic	Quadratic::operator+( const Quadratic& q ) const
{
    return Quadratic(m_a + q.m_a, m_b + q.m_b, m_c + q.m_c);
}

Quadratic	Quadratic::operator-( const Quadratic& q ) const
{
    return Quadratic(m_a - q.m_a, m_b - q.m_b, m_c - q.m_c);
}
