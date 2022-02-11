/***************************************************************************

    file                 : OsgWheel.cpp
    created              : Mon Aug 21 18:24:02 CEST 2012
    copyright            : (C) 2012 by Gaëtan André
    email                : gaetan.andre@gmail.com
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

/* Portability don't build with MingW 4.8 */
#ifdef _MSC_VER
#include <portability.h>
#endif // _MSC_VER

#include "OsgLoader.h"
#include "OsgWheel.h"
#include <osgDB/WriteFile>
#include <osgDB/FileUtils>

osg::ref_ptr<osg::Node> SDWheels::initWheels(tCarElt *car,void *handle)
{
    this->car = car;

    this->brakes.setCar(car);

    const char *wheelFront3DModFileNamePrfx =
            GfParmGetStr(handle, SECT_GROBJECTS, PRM_FRONT_WHEEL_3D, "");
    const char *wheelRear3DModFileNamePrfx =
            GfParmGetStr(handle, SECT_GROBJECTS, PRM_REAR_WHEEL_3D, "");
    const char *wheel3DModFileNamePrfx =
            GfParmGetStr(handle, SECT_GROBJECTS, PRM_WHEEL_3D, "wheel");

    if (*wheelFront3DModFileNamePrfx)
    {
        wheels[FRNT_RGT] = initWheel(FRNT_RGT, wheelFront3DModFileNamePrfx);
        wheels[FRNT_LFT] = initWheel(FRNT_LFT, wheelFront3DModFileNamePrfx);
    }
    else
    {
        wheels[FRNT_RGT] =initWheel(FRNT_RGT, wheel3DModFileNamePrfx);
        wheels[FRNT_LFT] =initWheel(FRNT_LFT, wheel3DModFileNamePrfx);
    }

    if (*wheelRear3DModFileNamePrfx)
    {
        wheels[REAR_RGT] =initWheel(REAR_RGT, wheelRear3DModFileNamePrfx);
        wheels[REAR_LFT] =initWheel(REAR_LFT, wheelRear3DModFileNamePrfx);
    }
    else
    {
        wheels[REAR_RGT] =initWheel(REAR_RGT, wheel3DModFileNamePrfx);
        wheels[REAR_LFT] =initWheel(REAR_LFT, wheel3DModFileNamePrfx);
    }

    osg::ref_ptr<osg::Group> group = new osg::Group;

    for(int i=0;i<4;i++)
    {
        group->addChild(wheels[i]);
    }

    return group.get();
}

osg::ref_ptr<osg::MatrixTransform> SDWheels::initWheel(int wheelIndex, const char * wheel_mod_name)
{
    osgLoader loader;
    char wheel_file_name[32];
    static const int MaxPathSize = 512;
    char buf[MaxPathSize];

    const bool bCustomSkin = strlen(this->car->_skinName) != 0;

    std::string bSkinName = "";
    std::string TmpPath = GetDataDir();

    if (bCustomSkin)
    {
        snprintf(buf, MaxPathSize, "cars/models/%s/wheel3d-%s.png", car->_carName, car->_skinName);
        bSkinName = TmpPath + buf;
        bool exist = osgDB::fileExists(bSkinName);
        GfLogInfo("Car Texture = %s\n", bSkinName.c_str());

        if (!exist)
                bSkinName="";
        else
        {
            snprintf(buf, MaxPathSize, "wheel3d-%s", car->_skinName);
            bSkinName = buf;
        }

        GfLogInfo("Car Texture = %s\n", bSkinName.c_str());
    }
    std::string strTPath;
    snprintf(buf, MaxPathSize, "drivers/%s/%d/", car->_modName, car->_driverIndex);
    strTPath = TmpPath+buf;
    loader.AddSearchPath(strTPath);

    snprintf(buf, MaxPathSize, "cars/models/%s/", car->_carName);
    strTPath = TmpPath+buf;
    loader.AddSearchPath(strTPath);
    GfLogDebug("Chemin Textures : %s\n", strTPath.c_str());

    snprintf(buf, MaxPathSize, "data/objects/");
    strTPath = TmpPath+buf;
    loader.AddSearchPath(strTPath);

    snprintf(buf, MaxPathSize, "data/textures/");
    strTPath = TmpPath+buf;
    loader.AddSearchPath(strTPath);

    wheels_switches[wheelIndex] = new osg::Switch;

    // Load speed-dependant 3D wheel model if available
    for(int j=0;j<4;j++)
    {
        osg::ref_ptr<osg::Node> wheel = 0;
        if (wheel_mod_name && strlen(wheel_mod_name))
        {
            snprintf(wheel_file_name, 32, "%s%d.acc", wheel_mod_name, j);
            wheel = loader.Load3dFile(wheel_file_name, true, bSkinName);
            wheels_switches[wheelIndex]->addChild(wheel.get(), false);
        }
    }

    osg::ref_ptr<osg::MatrixTransform> whlsize = new osg::MatrixTransform;
    float wheelRadius = car->_rimRadius(wheelIndex) + car->_tireHeight(wheelIndex);
    float w = car->_tireWidth(wheelIndex);
    float dia = wheelRadius * 2;

    osg::Matrix wheelsize = osg::Matrix::scale(dia, w, dia);

    whlsize->setMatrix(wheelsize);
    whlsize->addChild(wheels_switches[wheelIndex]);

    if(wheelIndex == FRNT_RGT || wheelIndex == REAR_RGT )
    {
        osg::ref_ptr<osg::MatrixTransform> flipright = new osg::MatrixTransform;
        flipright->setMatrix(osg::Matrix::rotate(osg::PI, osg::Z_AXIS));
        flipright->addChild(whlsize.get());
        whlsize = flipright.get();
    }

    osg::ref_ptr<osg::MatrixTransform> transform1 = new osg::MatrixTransform;
    transform1->addChild(whlsize.get());

    osg::ref_ptr<osg::MatrixTransform> transform2 = new osg::MatrixTransform;
    transform2->addChild(transform1.get());

    //initiating brakes
    transform2->addChild(this->brakes.initBrake(wheelIndex));

    return transform2.get();
}

void SDWheels::updateWheels()
{
    int j;
    static float maxVel[3] = { 20.0, 40.0, 70.0 };

    brakes.updateBrakes();

    for(int i=0; i<4; i++)
    {
        osg::Matrix spinMatrix = osg::Matrix::rotate(car->priv.wheel[i].relPos.ay, osg::Y_AXIS);

        osg::Matrix posMatrix = osg::Matrix::translate(car->priv.wheel[i].relPos.x, car->priv.wheel[i].relPos.y, car->priv.wheel[i].relPos.z);

        osg::Matrix camberDirMatrix = osg::Matrix::rotate(car->priv.wheel[i].relPos.ax, osg::X_AXIS, //camber
                                                          0.0, osg::Y_AXIS,
                                                          car->priv.wheel[i].relPos.az, osg::Z_AXIS ); //direction

        posMatrix = camberDirMatrix * posMatrix;
        osg::MatrixTransform * trans = dynamic_cast<osg::MatrixTransform *>(wheels[i]->getChild(0));
        trans->setMatrix(spinMatrix);
        wheels[i]->setMatrix(posMatrix);

        for (j = 0; j < 3; j++)
        {
            if (fabs(car->_wheelSpinVel(i)) < maxVel[j])
                break;
        }

        this->wheels_switches[i]->setSingleChildOn(j);
    }
}
