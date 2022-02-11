/***************************************************************************

    file        : Vec2d.h
    created     : 14 Jan 2019
    copyright   : (C) 2019 D. Schellhammer

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef _VEC2D_H_
#define _VEC2D_H_

#include "Vec2.h"

class Vec2d : public Vec2<double>
{
public:
    Vec2d() {}
    Vec2d(const Vec2<double>& vec) : Vec2<double>(vec) {}
    Vec2d(double x, double y) : Vec2<double>(x, y) {}
};

#endif // _VEC2D_H_
