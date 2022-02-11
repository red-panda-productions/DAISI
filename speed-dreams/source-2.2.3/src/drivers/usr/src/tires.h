/***************************************************************************

    file                 : tires.h
    created              : 2017-03-21 07:39:00 UTC
    copyright            : (C) Daniel Schellhammer

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef _TIRES_H_
#define _TIRES_H_

#include <car.h>

class Tires
{
public:
    Tires();
    void   init(const tCarElt* car);
    void   update();
    double TyreConditionFront() const;
    double TyreConditionRear() const;
    double TyreCondition() const;
    double TyreTreadDepthFront() const;
    double TyreTreadDepthRear() const;
    double TyreTreadDepth() const;
    double frictionBalanceLR() const;
    double gripFactor() const { return mGripFactor; }
    double distLeft() const { return mDistLeft; }
    double wear() const { return mWear; }
    double avgWearPerMeter() const { return mAvgWearPerMeter; }
    void   setTYC(bool tyc) { HasTYC = tyc; }

private:
    double updateGripFactor() const;

    const  tCarElt* mCar;
    double mGripFactor;
    double mDistWhenNew;
    double mDistLeft;
    double mWear;
    double mAvgWearPerMeter;
    double mHotTemp;
    double mMuScale;
    double mAvgGrain;
    double HasTYC;
};

#endif // _TIRES_H_
