/***************************************************************************

    file        : Vec3d.h
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

#ifndef _VEC3D_H_
#define _VEC3D_H_

#include "Vec3.h"

class Vec3d : public Vec3<double>
{
public:
    Vec3d() {}
    Vec3d(const Vec3<double>& vec) : Vec3<double>(vec) {}
    Vec3d(double x, double y, double z) : Vec3<double>(x, y, z) {}
};

#endif // _VEC3D_H_
