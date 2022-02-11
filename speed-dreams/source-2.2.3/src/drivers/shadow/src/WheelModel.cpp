/***************************************************************************

    file        : WheelModel.cpp
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

#include <robottools.h>

#include "WheelModel.h"
#include "CarModel.h"
#include "Utils.h"

WheelModel::WheelModel()
:   _w(0),
    _X(0),
    _Y(0),
    _Z(0),
    _B(0),
    _C(0),
    _E(0),
    _MU(1),
    _x(0),
    _y(0),
    _z(0),
    _sx(0),
    _sy(0),
    _sa(0)
{
    memset( &_tp, 0, sizeof(_tp));
}

WheelModel::~WheelModel()
{
}

void    WheelModel::setWheel( int wheel )
{
    _w = wheel;
}

void    WheelModel::config( const tCarElt* car )
{
    config( car->_carHandle );

    _X -= (double)car->info.statGC.x;
    _Y -= (double)car->info.statGC.y;
    _Z -= (double)car->info.statGC.z;
}

void    WheelModel::config( void* hCar )
{
    static const char* axleSect[2] = {SECT_FRNTAXLE, SECT_REARAXLE};
    static const char* wheelSect[4] = {SECT_FRNTRGTWHEEL, SECT_FRNTLFTWHEEL, SECT_REARRGTWHEEL, SECT_REARLFTWHEEL};

    _X = GfParmGetNum(hCar, axleSect[_w/2], PRM_XPOS, (char*)NULL, 0.0f);
    _Y = GfParmGetNum(hCar, wheelSect[_w], PRM_YPOS, (char*)NULL, 0.0f);
    _Z = 0;

    _MU = GfParmGetNum(hCar, wheelSect[_w], PRM_MU, (char*)NULL, 1.0f);

    double  Ca      = GfParmGetNum(hCar, wheelSect[_w], PRM_CA, (char*)NULL, 30.0f);
    double  RFactor = GfParmGetNum(hCar, wheelSect[_w], PRM_RFACTOR, (char*)NULL, 0.8f);
    double  EFactor = GfParmGetNum(hCar, wheelSect[_w], PRM_EFACTOR, (char*)NULL, 0.7f);

    _C = 2 - asin(RFactor) * 2 / PI;
    _B = Ca / _C;
    _E = EFactor;

    double  rimDiam     = GfParmGetNum(hCar, wheelSect[_w], PRM_RIMDIAM,   (char*)NULL, 0.33f);
    double  tyreWidth   = GfParmGetNum(hCar, wheelSect[_w], PRM_TIREWIDTH, (char*)NULL, 0.145f);
    double  tyreRatio   = GfParmGetNum(hCar, wheelSect[_w], PRM_TIRERATIO, (char*)NULL, 0.75f);

    _R = rimDiam * 0.5f + tyreWidth * tyreRatio;
}

void    WheelModel::update( const tCarElt* car, const tSituation* sit, const CarModel& cm )
{
    updatePosition( car, sit );
    updateSlip( car, sit, cm );
}

void    WheelModel::updatePosition( const tCarElt* car, const tSituation* sit )
{
    const sgMat4& m = car->pub.posMat;
    _x = car->pub.DynGCg.pos.x + m[0][0] * _X + m[0][1] * _Y + m[0][2] * _Z;
    _y = car->pub.DynGCg.pos.y + m[1][0] * _X + m[1][1] * _Y + m[1][2] * _Z;
    _z = car->pub.DynGCg.pos.z + m[2][0] * _X + m[2][1] * _Y + m[2][2] * _Z;

    RtTrackGlobal2Local(car->pub.trkPos.seg, (tdble)_x, (tdble)_y, &_tp, TR_LPOS_SEGMENT);

    _vay = (_vay + car->_wheelSpinVel(_w)) * 0.5;
}

void    WheelModel::updateSlip( const tCarElt* car, const tSituation* sit, const CarModel& cm )
{
    double  zforce  = car->_reaction[_w];

    if( zforce == 0 )
    {
        // in air?
        _sx = _sy = _sa = 0;

        return;
    }

    if( car->pub.speed < 0.5 )
    {
        _sx = _w < 2 ? 0 : car->ctrl.accelCmd * 0.5;
        _sy = _sa = 0;

        return;
    }

    double  bvx = cm.VEL_L.x - cm.VEL_AZ * _Y;
    double  bvy = cm.VEL_L.y + cm.VEL_AZ * _X;
    double  bv  = hypot(bvx, bvy);

    double  waz = _w < 2 ? car->ctrl.steer * car->info.steerLock : 0;
    double  wrl = _vay * car->_wheelRadius(_w);

    if( bv < 0.000001f )
    {
        _sa = 0;
        _sx = wrl;
        _sy = 0;
    }
    else
    {
        _sa = atan2(bvy, bvx) - waz;
        NORM_PI_PI(_sa);

        double  vt = bvx * cos(waz) + bvy * sin(waz);
        _sx = (vt - wrl) / fabs(vt);
        _sy = sin(_sa); // should be tan(_sa)???
    }
}

