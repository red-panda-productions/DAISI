/***************************************************************************

    file                 : OsgCarLight.h
    created              : Tue Mar 31 15:32:14 CEST 2020
    .........            : ...2020 Ivan Mahonin
    email                : bh@icystar.com
    version              : $Id$

***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef _OSGCARLIGHT_H_
#define _OSGCARLIGHT_H_


#include <osg/Group>
#include <osg/StateSet>


enum CarLightType
{
    CAR_LIGHT_TYPE_NONE     = 0,
    CAR_LIGHT_TYPE_FRONT    = 1,
    CAR_LIGHT_TYPE_FRONT2   = 2,
    CAR_LIGHT_TYPE_REAR     = 3,
    CAR_LIGHT_TYPE_REAR2    = 4,
    CAR_LIGHT_TYPE_BRAKE    = 5,
    CAR_LIGHT_TYPE_BRAKE2   = 6,
    CAR_LIGHT_TYPE_REVERSE  = 7
};
enum { CAR_LIGHT_TYPE_COUNT = 8 };


class SDCar;


class SDCarLight {
private:
    CarLightType type;
    osg::ref_ptr<osg::Node> node;

public:
    SDCarLight() {}
    ~SDCarLight() {}
    
    CarLightType get_type() const { return type; }

    osg::ref_ptr<osg::Node> init(
        CarLightType type,
        osg::ref_ptr<osg::StateSet> state_set,
        const osg::Vec3d &position,
        const osg::Vec3d &normal,
        double size,
        int layers );

    void update(const SDCar &car);
};


class SDCarLights
{
private:
    osg::ref_ptr<osg::Group> lights_root;
    osg::ref_ptr<osg::StateSet> state_sets[CAR_LIGHT_TYPE_COUNT];

public:
    SDCarLights(): lights_root(new osg::Group) {}
    ~SDCarLights() {}

    void loadStates();
    
    osg::ref_ptr<osg::Group> getLightsRoot()
        { return lights_root; }
    osg::ref_ptr<osg::StateSet> getStateSet(CarLightType type)
        { return state_sets[(int)type > 0 && (int)type < CAR_LIGHT_TYPE_COUNT ? type : 0]; }
};

#endif
