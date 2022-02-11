/***************************************************************************

    file        : CarBounds2d.h
    created     : (C) 2017
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

#ifndef _CAR_BOUNDARY_2D_
#define _CAR_BOUNDARY_2D_

#include "car.h"
#include "Vec2d.h"

#include <vector>

class CarBounds2d
{
    Vec2d	pts[4];
    Vec2d	xAxis;	// direction that is forwards.
    Vec2d	yAxis;	// direction this is to the right.

public:
    enum
    {
        SIDE_FRONT,
        SIDE_REAR,
        SIDE_LEFT,
        SIDE_RIGHT,
    };

public:
    CarBounds2d( const tCarElt* car );

    const Vec2d& operator[]( int index ) const { return pts[index]; }

    double	distToSide( int side, double maxDist, const CarBounds2d& other ) const;
    double	distToSide( int side, double maxDist, const std::vector<Vec2d>& pts ) const;

    void	inflateSide( int sideX, double delta );
    void	inflate( double deltaX, double deltaY );
    void	inflate( double deltaX1, double deltaX2, double deltaY1, double deltaY2 );
    bool	contains( const Vec2d& pt ) const;
    bool	collidesWith( const CarBounds2d& other ) const;
    bool	collidesWith( const Vec2d& pt1, const Vec2d& pt2 ) const;
    bool	collidesWith( const std::vector<Vec2d>& pts, const Vec2d& filterPt, double filterDistSqLimit = -1 ) const;
};

#endif	// _CAR_BOUNDARY_2D_
