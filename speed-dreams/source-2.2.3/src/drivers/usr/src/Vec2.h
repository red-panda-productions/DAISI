/***************************************************************************

    file        : Vec2.h
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

#ifndef _VEC2_H_
#define _VEC2_H_

#include <math.h> // not <cmath> for VS 2013 compatibility

template <class T> class Vec2
{
public:
    Vec2() { x = y = 0; };
    Vec2(T xValue, T yValue)
    {
        x = xValue;
        y = yValue;
    }

    T angle()
    {
        return atan2(y, x);
    }

    T len()
    {
        return sqrt(x * x + y * y);
    }

    void normalize()
    {
        T l = len();
        if (l != 0)
        {
            x /= l;
            y /= l;
        }
    }

    // "Cross Product" with z=0, good to check the order of the vectors
    T fakeCrossProduct(const Vec2* vec) const
    {
        return x * vec->y - y * vec->x;
    }

    Vec2 operator+(const Vec2 &vec) const
    {
        return Vec2(x + vec.x, y + vec.y);
    }

    void operator+=(const Vec2 &vec)
    {
        x += vec.x;
        y += vec.y;
    }

    Vec2 operator-(const Vec2 &vec) const
    {
        return Vec2(x - vec.x, y - vec.y);
    }

    void operator-=(const Vec2 &vec)
    {
        x -= vec.x;
        y -= vec.y;
    }

    Vec2 operator-(void) const
    {
        return Vec2(-x, -y);
    }

    Vec2 operator*(const T &fact) const
    {
        return Vec2(x * fact, y * fact);
    }

    // Dot (scalar) product
    T operator*(const Vec2 &vec) const
    {
        return x * vec.x + y * vec.y;
    }

    void operator*=(const T &fact)
    {
        x *= fact;
        y *= fact;
    }

    Vec2 operator/(const T &fact) const
    {
        return Vec2(x / fact, y / fact);
    }

    void operator/=(const T &fact)
    {
        x /= fact;
        y /= fact;
    }

    int operator==(const Vec2 &vec) const
    {
        if (x == vec.x && y == vec.y)
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }

    int operator!=(const Vec2 &vec) const
    {
        return !(*this == vec);
    }

    T x, y;
};

#endif // _VEC2_H_
