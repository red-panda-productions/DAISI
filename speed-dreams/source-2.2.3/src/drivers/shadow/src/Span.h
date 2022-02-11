/***************************************************************************

    file        : Span.h
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

#ifndef _SPAN_H_
#define _SPAN_H_

class Span
{
public:
    Span();
    Span( const Span& span );
    Span( double A, double B );
    ~Span();

    bool	IsNull() const;
    double	GetSize() const;

    void	Set( double x, double y );

    bool	Overlaps( const Span& span ) const;
    bool	Contains( const Span& span ) const;
    bool	Contains( double x ) const;

    Span	Intersect( const Span& span ) const;
    Span	Intersect( double A, double B ) const;

    void	Extend( double x );
    void	ExcludeLeftOf( double x );
    void	ExcludeRightOf( double x );

public:
    double	a;
    double	b;
};

#endif
