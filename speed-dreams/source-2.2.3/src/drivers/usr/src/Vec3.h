/***************************************************************************

    file        : Vec3.h
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

#ifndef _VEC3_H_
#define _VEC3_H_

#include "Vec2.h"

#include <math.h> // not <cmath> for VS 2013 compatibility

template <class T> class Vec3
{
public:
    Vec3() { x = y = z = 0; };
    Vec3(T xValue, T yValue, T zValue)
    {
        x = xValue;
        y = yValue;
        z = zValue;
    }

    Vec2<T> getVec2() const
    {
        return Vec2<T>(x, y);
    }

    T len()
    {
        return sqrt(x * x + y * y + z * z);
    }

    void normalize()
    {
        T l = len();
        if (l != 0)
        {
            x /= l;
            y /= l;
            z /= l;
        }
    }

    T dotProduct(const Vec3& vec) const
    {
        return x * vec.x + y * vec.y + z * vec.z;
    }

    Vec3 crossProduct(const Vec3& vec)
    {
        return Vec3(y * vec.z - z * vec.y, z * vec.x - x * vec.z, x * vec.y - y * vec.x);
    }

    Vec3 operator+(const Vec3& vec) const
    {
        return Vec3(x + vec.x, y + vec.y, z + vec.z);
    }

    void operator+=(const Vec3& vec)
    {
        x += vec.x;
        y += vec.y;
        z += vec.z;
    }

    Vec3 operator-(const Vec3& vec) const
    {
        return Vec3(x - vec.x, y - vec.y, z - vec.z);
    }

    void operator-=(const Vec3& vec)
    {
        x -= vec.x;
        y -= vec.y;
        z -= vec.z;
    }

    Vec3 operator-(void) const
    {
        return Vec3(-x, -y, -z);
    }

    Vec3 operator*(const T& fact) const
    {
        return Vec3(x * fact, y * fact, z * fact);
    }

    void operator*=(const T& fact)
    {
        x *= fact;
        y *= fact;
        z *= fact;
    }

    Vec3 operator/(const T& fact) const
    {
        return Vec3(x / fact, y / fact, z / fact);
    }

    void operator/=(const T& fact)
    {
        x /= fact;
        y /= fact;
        z /= fact;
    }

    T x, y, z;
};

#endif // _VEC3_H_
