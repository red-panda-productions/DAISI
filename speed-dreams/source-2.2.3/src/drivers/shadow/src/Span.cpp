/***************************************************************************

    file        : Span.cpp
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

#include "Utils.h"
#include "Span.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Span::Span()
:	a(0),
    b(0)
{
}

Span::Span( const Span& span )
:	a(span.a),
    b(span.b)
{
}

Span::Span( double A, double B )
:	a(A),
    b(B)
{
}

Span::~Span()
{
}

bool Span::IsNull() const
{
    return a >= b;
}

double Span::GetSize() const
{
    return b - a;
}

void Span::Set( double x, double y )
{
    a = x;
    b = y;
}

bool Span::Overlaps( const Span& span ) const
{
    if( IsNull() || span.IsNull() )
        return false;

    double	newA = MX(a, span.a);
    double	newB = MN(b, span.b);
    return newA < newB;
}

bool Span::Contains( const Span& span ) const
{
    return !span.IsNull() && a <= span.a && span.b <= b;
}

bool Span::Contains( double x ) const
{
    return a <= x && x < b;
}

Span Span::Intersect( const Span& span ) const
{
    if( IsNull() || span.IsNull() )
        return Span(0, 0);

    double	newA = MX(a, span.a);
    double	newB = MN(b, span.b);
    return Span(newA, newB);
}

Span Span::Intersect( double A, double B ) const
{
    if( IsNull() || A > B )
        return Span(0, 0);

    double	newA = MX(a, A);
    double	newB = MN(b, B);
    return Span(newA, newB);
}

void Span::Extend( double x )
{
    if( IsNull() )
        a = b = x;
    else if( x < a )
        a = x;
    else if( x > b )
        b = x;
}

void Span::ExcludeLeftOf( double x )
{
    if( a < x )
        a = x;
}

void Span::ExcludeRightOf( double x )
{
    if( b > x )
        b = x;
}
