/***************************************************************************

    file                 : OsgCar.h
    created              : Mon Aug 21 18:24:02 CEST 2012
    copyright            : (C)2012 by Gaétan André, (C)2014 Xavier Bertaux
    email                : gaetan.andré@gmail.com
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

#ifndef _OSGCAR_H_
#define _OSGCAR_H_

#include <car.h>
#include <raceman.h>
#include <vector>

#include <osg/TextureCubeMap>
#include <osg/LOD>

#include "OsgDriver.h"
#include "OsgWheel.h"
#include "OsgCarLight.h"


class SDCarShader;
class SDReflectionMapping;
class SDWheels;

class SDCar
{
private :
    osg::ref_ptr<osg::MatrixTransform> car_branch;
    osg::ref_ptr<osg::MatrixTransform> lights_branch;
    osg::ref_ptr<osg::Group> car_shaded_body;
    osg::ref_ptr<osg::Group> car_root;
    osg::ref_ptr<osg::Switch> pWing3;
    osg::ref_ptr<osg::Switch> pDriver;
    osg::ref_ptr<osg::LOD> pSteer;
    osg::ref_ptr<osg::MatrixTransform> Steer_branch;
    //osg::ref_ptr<osg::Vec3Array> shadowVertices;
    //osg::ref_ptr<osg::Geometry> quad;
    //osg::ref_ptr<osg::Node> initOcclusionQuad(tCarElt *car);

    std::vector<SDCarLight> lights;

    tCarElt     *car;

    SDWheels    wheels;
    SDCarShader *shader;
    SDReflectionMapping *reflectionMapping;

    int reflectionMappingMethod;
    void setReflectionMap(osg::ref_ptr<osg::Texture> map);

    void loadCarLights(tCarElt *Car);

public :
    SDCar(void);
    ~SDCar(void);
    osg::ref_ptr<osg::Node> loadCar(tCarElt *Car, bool tracktype, bool subcat, int carshader);
    bool isCar(tCarElt*c);

    bool    _cockpit;
    bool    _driver;
    bool    _wing1;
    bool    _wing3;
    bool    _steer;

    int     _carShader;

    SDReflectionMapping *getReflectionMap();

    int getReflectionMappingMethod();

    tCarElt *getCar() { return car; }
    const tCarElt *getCar() const { return car; }

    void markCarCurrent(tCarElt *Car);
    void updateCar();
    void updateShadingParameters(const osg::Matrixf &modelview);
};

class SDCars
{
private :
    std::vector<SDCar *> the_cars;
    osg::ref_ptr<osg::Group> cars_branch;
    osg::ref_ptr<osg::Group> shadow_branch;
    tSituation *situation;

    void addSDCar(SDCar * car);

public :
    SDCars(void);
    ~SDCars(void);

    void loadCars(tSituation * pSituation, bool trackType, bool subCat);
    void updateCars();
    void markCarCurrent(tCarElt*car);
    SDCar *getCar(tCarElt*car);
    void unLoad();
    void updateShadingParameters(const osg::Matrixf &modelview);
    osg::ref_ptr<osg::Node> getShadowNode() { return shadow_branch.get(); }
    osg::ref_ptr<osg::Node> getCarsNode() { return cars_branch.get(); }

};

#endif /* _OSGCAR_H_ */
