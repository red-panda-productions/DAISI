/***************************************************************************

    file        : WheelModel.h
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

#ifndef _WHEEL_MODEL_H_
#define _WHEEL_MODEL_H_

#include <robot.h>
#include <track.h>
#include <math.h>

//#include "CarModel.h"
class CarModel;

class WheelModel
{
public:
    WheelModel();
    ~WheelModel();

    void    setWheel( int wheel );
    void    config( const tCarElt* car );
    void    config( void* hCar );
    void    update( const tCarElt* car, const tSituation* s, const CarModel& cm );

    double	slipX() const { return _sx; }
    double	slipY() const { return _sy; }
    double	slip() const  { return hypot(_sx, _sy); }

    double  radius() const { return _R; }

private:
    void    updatePosition( const tCarElt* car, const tSituation* s );
    void    updateSlip( const tCarElt* car, const tSituation* s, const CarModel& cm );

private:
    int         _w;     // which wheel this is

    double      _X;     // static x position of wheel relative to car GC.
    double      _Y;     // static y position of wheel relative to car GC.
    double      _Z;     // static z position of wheel relative to car GC.

    double      _B;     // magic formula constant
    double      _C;     // magic formula constant
    double      _E;     // magic formula constant

    double      _MU;    // tyre mu.

    double      _R;     // nominal wheel & tyre radius.

    double      _x;     // global x position of wheel
    double      _y;     // global y position of wheel
    double      _z;     // global z position of wheel
    double		_vay;	// wheel spin speed
    tTrkLocPos  _tp;    // track position of wheel

    double      _sx;    // slip in x direction
    double      _sy;    // slip in y direction
    double      _sa;    // slip angle (angle of wheel to wheel body velocity)
};

#endif  // _WHEEL_MODEL_H_
