/***************************************************************************

    file                 : OsgCar.cpp
    created              : Mon Aug 21 18:24:02 CEST 2012
    copyright            : (C)2012 by Gaétan André, (C)2014 Xavier Bertaux
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

#include <osg/MatrixTransform>
#include <osg/Switch>
#include <osg/Group>
#include <osgUtil/Simplifier>
#include <osgViewer/Viewer>
#include <osg/Program>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/Texture2D>
#include <osg/AlphaFunc>
#include <osg/BlendFunc>
#include <osg/Material>
#include <osg/Depth>
#include <osgDB/ReadFile>
#include <osgDB/WriteFile>

#include <portability.h>
#include <robottools.h>

#include "OsgLoader.h"
#include "OsgCar.h"
#include "OsgMath.h"
#include "OsgScreens.h"
#include "OsgRender.h"
#include "OsgMain.h"
#include "OsgSky.h"
#include "OsgShader.h"
#include "OsgReflectionMapping.h"
#include "OsgNodeMask.h"

class SDRender;

SDCar::SDCar(void) :
    car_branch(NULL),
    car_root(NULL),
    pWing3(NULL),
    pDriver(NULL),
    pSteer(NULL),
    Steer_branch(NULL),
    shader(NULL),
    reflectionMapping(NULL)
{
    _cockpit = false;
    _driver = false;
    _wing1 = false;
    _wing3 = false;
    _steer = false;

    _carShader = 0;
}

SDCar::~SDCar(void)
{
    if (lights_branch)
        ((SDCarLights*)getCarLights())->getLightsRoot()->removeChild(lights_branch);

    if(car_root != NULL)
    {
        car_root->removeChildren(0, car_root->getNumChildren());
        car_root = NULL;
    }

    delete shader;
    delete reflectionMapping;
}

void SDCar::loadCarLights(tCarElt *Car)
{
    SDCarLights *carLights = (SDCarLights*)getCarLights();

    if (lights_branch)
        carLights->getLightsRoot()->removeChild(lights_branch);
    lights_branch = new osg::MatrixTransform;

    char path[1024] = {};
    void *handle = Car->_carHandle;
    snprintf(path, 1023, "%s/%s", SECT_GROBJECTS, SECT_LIGHT);
    int lightNum = GfParmGetEltNb(handle, path);
    for (int i = 0; i < lightNum; i++) {
        snprintf(path, 1023, "%s/%s/%d", SECT_GROBJECTS, SECT_LIGHT, i + 1);

        osg::Vec3d position;
        position[0] = GfParmGetNum(handle, path, PRM_XPOS, NULL, 0);
        position[1] = GfParmGetNum(handle, path, PRM_YPOS, NULL, 0);
        position[2] = GfParmGetNum(handle, path, PRM_ZPOS, NULL, 0);

        osg::Vec3d normal;
        normal[0] = position[0] > 0 ? 1 : -1;

        const char *typeName = GfParmGetStr(handle, path, PRM_TYPE, "");
        CarLightType type = CAR_LIGHT_TYPE_NONE;
        if (!strcmp(typeName, VAL_LIGHT_HEAD1)) {
            type = CAR_LIGHT_TYPE_FRONT;
        } else
        if (!strcmp(typeName, VAL_LIGHT_HEAD2)) {
            type = CAR_LIGHT_TYPE_FRONT2;
        } else
        if (!strcmp(typeName, VAL_LIGHT_BRAKE)) {
            type = CAR_LIGHT_TYPE_BRAKE;
        } else
        if (!strcmp(typeName, VAL_LIGHT_BRAKE2)) {
            type = CAR_LIGHT_TYPE_BRAKE2;
        } else
        if (!strcmp(typeName, VAL_LIGHT_REAR)) {
            type = CAR_LIGHT_TYPE_REAR;
        } else
        if (!strcmp(typeName, VAL_LIGHT_REVERSE)) {
            type = CAR_LIGHT_TYPE_REVERSE;
        }

        double size = GfParmGetNum(handle, path, PRM_SIZE, NULL, 0.2);

        osg::ref_ptr<osg::StateSet> state_set = carLights->getStateSet(type);

        SDCarLight light;
        lights_branch->addChild( light.init(type, state_set, position, normal, size, 4) );
        lights.push_back(light);
    }

    lights_branch->setMatrix( car_branch->getMatrix() );
    carLights->getLightsRoot()->addChild(lights_branch);
}

osg::ref_ptr<osg::Node> SDCar::loadCar(tCarElt *Car, bool tracktype, bool subcat, int carshader)
{
    this->car_branch = new osg::MatrixTransform;
    this->car_shaded_body = new osg::Group;
    this->car_root = new osg::Group;
    this->car = Car;

    /* Schedule texture mapping if we are using a custom skin and/or a master 3D model */
    const bool bMasterModel = strlen(this->car->_masterModel) != 0;
    const bool bCustomSkin = strlen(this->car->_skinName) != 0;

    std::string bSkinName = "";
    std::string bCarName = "";

    static const int nMaxTexPathSize = 512;
    char buf[nMaxTexPathSize];
    char path[nMaxTexPathSize];

    int index;
    void *handle;
    const char *param;

    int nranges = 0;

    _carShader = carshader;
    GfLogDebug("Graphic options car shader = %u\n", _carShader);

    osgLoader loader;

    std::string TmpPath = GetDataDir();
    std::string strTPath;

    index = car->index;	/* current car's index */
    handle = car->_carHandle;

    GfLogInfo("[gr] Init(%d) car %s for driver %s index %d\n", index, car->_carName, car->_modName, car->_driverIndex);
    GfLogInfo("[gr] Init(%d) car %s MasterModel name\n", index, car->_masterModel);
    GfLogInfo("[gr] Init(%d) car %s CustomSkin name\n", index, car->_skinName);

    snprintf(buf, nMaxTexPathSize, "%sdrivers/%s/%d/",
             GfDataDir(), car->_modName, car->_driverIndex);
    strTPath = TmpPath+buf;
    loader.AddSearchPath(strTPath);

    snprintf(buf, nMaxTexPathSize, "%sdrivers/%s/%s/",
             GfDataDir(), car->_modName, car->_carName);
    strTPath = TmpPath+buf;
    loader.AddSearchPath(strTPath);

    snprintf(buf, nMaxTexPathSize, "%sdrivers/%s/",
             GfDataDir(), car->_modName);
    strTPath = TmpPath+buf;
    loader.AddSearchPath(strTPath);

    snprintf(buf, nMaxTexPathSize, "drivers/%s/%d/%s/",
             car->_modName, car->_driverIndex, car->_carName);
    strTPath = TmpPath+buf;
    loader.AddSearchPath(strTPath);

    snprintf(buf, nMaxTexPathSize, "drivers/%s/%d/",
             car->_modName, car->_driverIndex);
    strTPath = TmpPath+buf;
    loader.AddSearchPath(strTPath);

    snprintf(buf, nMaxTexPathSize, "drivers/%s/%s/",
             car->_modName, car->_carName);
    strTPath = TmpPath+buf;
    loader.AddSearchPath(strTPath);

    snprintf(buf, nMaxTexPathSize, "drivers/%s/", car->_modName);
    strTPath = TmpPath+buf;
    loader.AddSearchPath(strTPath);

    snprintf(buf, nMaxTexPathSize, "cars/models/%s/", car->_carName);
    strTPath = TmpPath+buf;
    loader.AddSearchPath(strTPath);

    snprintf(buf, nMaxTexPathSize, "data/objects/");
    strTPath = TmpPath+buf;
    loader.AddSearchPath(strTPath);

    snprintf(buf, nMaxTexPathSize, "data/textures/");
    strTPath = TmpPath+buf;
    loader.AddSearchPath(strTPath);

    /* loading raw car level 0*/
    //  selIndex = 0; 	/* current selector index */
    snprintf(buf, nMaxTexPathSize, "%s.ac",
             car->_carName); /* default car 3D model file */
    snprintf(path, nMaxTexPathSize, "%s/%s/1", SECT_GROBJECTS, LST_RANGES);

    std::string strPath = GetDataDir();

    if(bMasterModel)
        snprintf(buf, nMaxTexPathSize, "cars/models/%s/%s.acc", car->_masterModel, car->_masterModel);
    else
        snprintf(buf, nMaxTexPathSize, "cars/models/%s/%s.acc", car->_carName, car->_carName);

    strPath+=buf;

    std::string name = car->_carName;

    osg::ref_ptr<osg::Group> gCar = new osg::Group;
    gCar->setName("CAR");
    osg::ref_ptr<osg::Switch> pBody =new osg::Switch;
    pBody->setName("COCK");
    osg::ref_ptr<osg::Node> pCar = new osg::Node;
    osg::ref_ptr<osg::Node> pCockpit = new osg::Node;
    osg::ref_ptr<osg::Switch> pWing = new osg::Switch;
    pWing->setName("WING");
    pWing3 = new osg::Switch;
    pWing3->setName("WINGREAR");
    pDriver = new osg::Switch;
    pDriver->setName("DRIVER");
    pSteer = new osg::LOD;
    pSteer->setName("STEER");
    Steer_branch = new osg::MatrixTransform;
    Steer_branch->setName("STEERBRANCHE");

    //strPath+=buf;
    GfLogDebug("Chemin Textures : %s\n", strTPath.c_str());

    //osg::ref_ptr<osg::Node> Car = new osg::Node;
    if (bMasterModel)
        bSkinName = name;

    if (bCustomSkin)
    {
        snprintf(buf, nMaxTexPathSize, "%s-%s", car->_carName, car->_skinName);
        bSkinName = buf;
        snprintf(buf, nMaxTexPathSize, "%s.png", car->_carName);
        bCarName = buf;
        GfLogDebug("Car Texture = %s - Car Name = %s\n", bSkinName.c_str(), bCarName.c_str());
    }

    pCar = loader.Load3dFile(strPath, true, bCarName, bSkinName);

    GfLogDebug("Load Car ACC !\n");

    /* Set a selector on the wing type MPA*/
    snprintf(path, nMaxTexPathSize, "%s/%s", SECT_GROBJECTS, SECT_WING_MODEL);
    param = GfParmGetStr(handle, path, PRM_WING_1, NULL);
    if (param)
    {
        osg::ref_ptr<osg::Node> pWin1 = new osg::Node;
        osg::ref_ptr<osg::Node> pWin2 = new osg::Node;
        osg::ref_ptr<osg::Node> pWin3 = new osg::Node;

        _wing1 = true;

        std::string tmp = GetDataDir();
        snprintf(buf, nMaxTexPathSize, "cars/models/%s/", car->_carName);
        tmp = tmp+buf;

        param = GfParmGetStr(handle, path, PRM_WING_1, NULL);
        strPath=tmp+param;
        pWin1 = loader.Load3dFile(strPath, true, bCarName, bSkinName);
        pWin1->setName("WING1");
        GfLogDebug("Load Wing1 ACC ! %s\n", strPath.c_str() );

        param = GfParmGetStr(handle, path, PRM_WING_2, NULL);
        strPath=tmp+param;
        pWin2 = loader.Load3dFile(strPath, true, bCarName, bSkinName);
        pWin2->setName("WING2");
        GfLogDebug("Load Wing2 ACC ! %s\n", strPath.c_str());

        param = GfParmGetStr(handle, path, PRM_WING_3, NULL);
        strPath=tmp+param;
        pWin3 = loader.Load3dFile(strPath, true, bCarName, bSkinName);
        pWin3->setName("WING3");
        GfLogDebug("Load Wing3 ACC ! %s\n", strPath.c_str());

        pWing->addChild(pWin1.get(), false);
        pWing->addChild(pWin2.get(), false);
        pWing->addChild(pWin3.get(), true);

        GfLogInfo("tracktype = %d - subcat = %d\n", tracktype, subcat);

        if (tracktype == false)
        {
            pWing->setSingleChildOn(2);
            GfLogInfo("Activate Wing Road !\n");
        }
        else
        {
            if(subcat == false)
            {
                pWing->setSingleChildOn(0);
                GfLogInfo("Activate Wing Short !\n");
            }
            else
            {
                pWing->setSingleChildOn(1);
                GfLogInfo("Activate Wing Long !\n");
            }
        }
    }

    // Separate rear wing models for animation according to rear wing angle ...
    snprintf(path, nMaxTexPathSize, "%s/%s", SECT_GROBJECTS, LST_REARWING);
    nranges = GfParmGetEltNb(handle, path) + 1;

    if (nranges > 1)
    {
        _wing3 = true;
        std::string tmp = GetDataDir();
        snprintf(buf, nMaxTexPathSize, "cars/models/%s/", car->_carName);
        tmp = tmp+buf;

        // Add the rearwings
        for (int i = 1; i < nranges; i++)
        {
            osg::ref_ptr<osg::Node> pWing1_branch = new osg::Node;
            snprintf(path, nMaxTexPathSize, "%s/%s/%d", SECT_GROBJECTS, LST_REARWING, i);
            param = GfParmGetStr(handle, path, PRM_REARWINGMODEL, "");

            strPath = tmp+param;
            pWing1_branch = loader.Load3dFile(strPath, true, bCarName, bSkinName);
            GfLogDebug("Loading Wing animate %i - %s !\n", i, strPath.c_str());

            pWing3->addChild(pWing1_branch.get());
            strPath ="";
        }

        pWing3->setSingleChildOn(0);
        GfLogDebug("Rear Wing angle Loaded\n");
    }


    // Cockpit separate object loaded  ...
    snprintf(path, nMaxTexPathSize, "%s/%s", SECT_GROBJECTS, SECT_COCKPIT);
    param = GfParmGetStr(handle, path, PRM_MODELCOCKPIT, NULL);

    if (param)
    {
        _cockpit = true;
        std::string tmp = GetDataDir();
        snprintf(buf, nMaxTexPathSize, "cars/models/%s/", car->_carName);
        tmp = tmp+buf;

        strPath= tmp+param;

        pCockpit = loader.Load3dFile(strPath, true, bCarName, bSkinName);
        GfLogDebug("Cockpit loaded = %s !\n", strPath.c_str());
    }

    pBody->addChild(pCockpit.get(), false);

    /* add Steering Wheel 0 (if one exists) */
    snprintf(path, nMaxTexPathSize, "%s/%s", SECT_GROBJECTS, SECT_STEERWHEEL);
    param = GfParmGetStr(handle, path, PRM_SW_MODEL, NULL);

    if (param)
    {
        _steer = true;
        std::string tmpPath = GetDataDir();
        snprintf(buf, nMaxTexPathSize, "cars/models/%s/", car->_carName);
        tmpPath = tmpPath+buf;

        strPath = tmpPath + param;

        osg::ref_ptr<osg::Node> steerEntityLo = loader.Load3dFile(strPath, true, "", "");
        osg::ref_ptr<osg::MatrixTransform> steer_transform = new osg::MatrixTransform;

        tdble xpos = GfParmGetNum(handle, path, PRM_XPOS, NULL, 0.0);
        tdble ypos = GfParmGetNum(handle, path, PRM_YPOS, NULL, 0.0);
        tdble zpos = GfParmGetNum(handle, path, PRM_ZPOS, NULL, 0.0);
        float angl = GfParmGetNum(handle, path, PRM_SW_ANGLE, NULL, 0.0);
        //angl = SD_DEGREES_TO_RADIANS * angl;
        osg::Matrix pos = osg::Matrix::translate(xpos, ypos, zpos);
        osg::Matrix rot = osg::Matrix::rotate(angl, osg::Y_AXIS);

        pos = rot * pos;
        steer_transform->setMatrix(pos);

        steer_transform->addChild(steerEntityLo.get());

        pSteer->addChild(steer_transform.get(), 1.0f, FLT_MAX);
        osg::MatrixTransform * movt = new osg::MatrixTransform;
        osg::Matrix rot2 = osg::Matrix::rotate(0.3, osg::X_AXIS);

        Steer_branch->addChild(pSteer);
        Steer_branch->setMatrix(rot2);
        GfLogDebug("Low Steer Loading \n");

    }

    snprintf(path, nMaxTexPathSize, "%s/%s", SECT_GROBJECTS, SECT_STEERWHEEL);
    param = GfParmGetStr(handle, path, PRM_SW_MODELHR, NULL);

    if (param)
    {
        _steer = true;
        std::string tmpPath = GetDataDir();
        snprintf(buf, nMaxTexPathSize, "cars/models/%s/", car->_carName);
        tmpPath = tmpPath+buf;

        strPath = tmpPath + param;

        osg::ref_ptr<osg::Node> steerEntityHi = loader.Load3dFile(strPath, true, "", "");
        osg::ref_ptr<osg::MatrixTransform> steer_transform = new osg::MatrixTransform;

        tdble xpos = GfParmGetNum(handle, path, PRM_XPOS, NULL, 0.0);
        tdble ypos = GfParmGetNum(handle, path, PRM_YPOS, NULL, 0.0);
        tdble zpos = GfParmGetNum(handle, path, PRM_ZPOS, NULL, 0.0);
        float angl = GfParmGetNum(handle, path, PRM_SW_ANGLE, NULL, 0.0);
        //angl = SD_DEGREES_TO_RADIANS * angl;
        osg::Matrix pos = osg::Matrix::translate(xpos, ypos, zpos);
        osg::Matrix rot = osg::Matrix::rotate(angl, osg::Y_AXIS);

        pos = rot * pos;
        steer_transform->setMatrix(pos);

        steer_transform->addChild(steerEntityHi.get());
        pSteer->addChild(steer_transform.get(), 0.0f, 1.0f);
        GfLogDebug("High Steer Loading \n");
#if 0
        std::string Tpath = GetLocalDir();
        Tpath = Tpath+"/steer.osg";
        osgDB::writeNodeFile( *Steer_branch, Tpath);
#endif
    }

    // separate driver models for animation according to steering wheel angle ...
    snprintf(path, nMaxTexPathSize, "%s/%s", SECT_GROBJECTS, LST_DRIVER);
    nranges = GfParmGetEltNb(handle, path) + 1;

    if (nranges > 1)
    {
        _driver = true;
        int selIndex = 0;
        std::string tmp = GetLocalDir();
        std::string driver_path;

        // add the drivers
        for (int i = 1; i < nranges; i++)
        {
            osg::ref_ptr<osg::Node> driver_branch = new osg::Node;
            osg::ref_ptr<osg::MatrixTransform> position = new osg::MatrixTransform;

            snprintf(buf, nMaxTexPathSize, "%s/%s/%d", SECT_GROBJECTS, LST_DRIVER, i);
            param = GfParmGetStr(handle, buf, PRM_DRIVERMODEL, "");

            tdble xpos = GfParmGetNum(handle, buf, PRM_XPOS, NULL, 0.0);
            tdble ypos = GfParmGetNum(handle, buf, PRM_YPOS, NULL, 0.0);
            tdble zpos = GfParmGetNum(handle, buf, PRM_ZPOS, NULL, 0.0);
            osg::Matrix pos = osg::Matrix::translate(xpos, ypos, zpos);

            position->setMatrix(pos);

            driver_path = tmp+param;
            driver_branch = loader.Load3dFile(driver_path, true, bCarName, bSkinName);
            GfLogDebug("Loading Animated Driver %i - %s \n", i, driver_path.c_str());

            position->addChild(driver_branch.get());
            pDriver->addChild(position.get());
            driver_path ="";

            selIndex++;
        }

        pDriver->setSingleChildOn(0);
    }

    car_shaded_body->addChild(pCar.get());

    if(_wing1)
        car_shaded_body->addChild(pWing.get());

    if(_wing3)
       car_shaded_body->addChild(pWing3.get());

    gCar->addChild(car_shaded_body.get());
    gCar->addChild(pDriver.get());
    gCar->addChild(pSteer.get());


    pBody->addChild(gCar.get(), true);
    pBody->setSingleChildOn(1);

    osg::ref_ptr<osg::MatrixTransform> transform1 = new osg::MatrixTransform;

    transform1->addChild(pBody.get());

    // GfOut("loaded car %d",pCar.get());
    this->car_branch = transform1.get();

    //wheels = new SDWheels;
    this->car_branch->addChild(wheels.initWheels(car,handle));

    this->car_root = new osg::Group;
    car_root->addChild(car_branch);

    this->shader = new SDCarShader(car_shaded_body.get(), this);

    if (_carShader > 1)
        this->reflectionMappingMethod = REFLECTIONMAPPING_DYNAMIC;
    else
        this->reflectionMappingMethod = REFLECTIONMAPPING_OFF;

    this->reflectionMapping = new SDReflectionMapping(this);
    this->setReflectionMap(this->reflectionMapping->getReflectionMap());

    loadCarLights(Car);

    return this->car_root;
}

bool SDCar::isCar(tCarElt*c)
{
    return c==car;
}
SDReflectionMapping *SDCar::getReflectionMap()
{
    return this->reflectionMapping;
}

int SDCar::getReflectionMappingMethod()
{
    return this->reflectionMappingMethod;
}

/*#define GR_SHADOW_POINTS 6
#define MULT 1.1
osg::ref_ptr<osg::Node> SDCar::initOcclusionQuad(tCarElt *car)
{
    osg::Vec3f vtx;
    osg::Vec2f tex;
    float x;
    int i;

    char buf[512];
    std::string TmpPath = GetDataDir();

    //  GfOut("\n################## LOADING SHADOW ###############################\n");
    std::string shadowTextureName = GfParmGetStr(car->_carHandle, SECT_GROBJECTS, PRM_SHADOW_TEXTURE, "");

    snprintf(buf, sizeof(buf), "cars/models/%s/", car->_carName);
    if (strlen(car->_masterModel) > 0) // Add the master model path if we are using a template.
        snprintf(buf + strlen(buf), sizeof(buf) - strlen(buf), "cars/models/%s/", car->_masterModel);

    std::string dir = buf;
    shadowTextureName = TmpPath +dir+shadowTextureName;

    // GfOut("\n lepath = %s\n",shadowTextureName.c_str());
    osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;
    osg::ref_ptr<osg::Vec2Array> texcoords = new osg::Vec2Array;
    vtx._v[2] = 0.0;

    for (i = 0, x = car->_dimension_x * MULT / 2.0; i < GR_SHADOW_POINTS / 2;
         i++, x -= car->_dimension_x * MULT / (float)(GR_SHADOW_POINTS - 2) * 2.0)
    {
        vtx._v[0] = x;
        tex._v[0] = 1.0 - (float)i / (float)((GR_SHADOW_POINTS - 2) / 2.0);

        vtx._v[1] = -car->_dimension_y * MULT / 2.0;
        vertices->push_back(vtx);
        tex._v[1] = 0.0;
        texcoords->push_back(tex);

        vtx._v[1] = car->_dimension_y * MULT / 2.0;
        vertices->push_back(vtx);
        tex._v[1] = 1.0;
        texcoords->push_back(tex);
    }

    osg::ref_ptr<osg::Vec3Array> normals = new osg::Vec3Array;
    normals->push_back( osg::Vec3(0.0f,0.0f, 1.0f) );

    osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
    colors->push_back( osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f) );

    quad = new osg::Geometry;
    quad->setVertexArray( vertices.get() );
    quad->setNormalArray( normals.get() );
    quad->setNormalBinding( osg::Geometry::BIND_OVERALL );
    quad->setColorArray( colors.get() );
    quad->setColorBinding( osg::Geometry::BIND_OVERALL );
    quad->setTexCoordArray( 0, texcoords.get() );
    quad->addPrimitiveSet( new osg::DrawArrays(GL_TRIANGLE_STRIP, 0, vertices->size()) );

    quad->setDataVariance(osg::Object::DYNAMIC);

    osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D;
    osg::ref_ptr<osg::Image> image = osgDB::readImageFile(shadowTextureName);
    texture->setImage( image.get() );

    osg::ref_ptr<osg::BlendFunc> blendFunc = new osg::BlendFunc;
    blendFunc->setFunction( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

    osg::ref_ptr<osg::Geode> root = new osg::Geode;
    root->addDrawable( quad.get() );

    osg::StateSet* stateset = root->getOrCreateStateSet();
    stateset->setRenderBinDetails( 2, "DepthSortedBin");
    stateset->setTextureAttributeAndModes(0, texture.get() );
    stateset->setAttributeAndModes( blendFunc );
    stateset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN );

    shadowVertices = vertices;

    //  GfOut("\n################## LOADED SHADOW ###############################\n");
    return root.get();
}*/

void SDCar::markCarCurrent(tCarElt *Car)
{
    if(this->car == Car)
    {
        car_branch->setNodeMask(NODE_MASK_CURCAR);
        lights_branch->setNodeMask(NODE_MASK_CURCAR);
        pDriver->setNodeMask(NODE_MASK_CURDRV);
    }
    else
    {
        car_branch->setNodeMask(NODE_MASK_ALL);
        lights_branch->setNodeMask(NODE_MASK_ALL);
        pDriver->setNodeMask(NODE_MASK_ALL);
    }
}

void SDCar::updateCar()
{
    osg::Vec3 p;
    float wingangle = this->car->_wingRCmd * 180 / PI;
    float steerangle = this->car->_steerCmd;

    p[0] = this->car->_pos_X;//+ car->_drvPos_x;
    p[1] = this->car->_pos_Y;//+car->_drvPos_y;
    p[2] = this->car->_pos_Z;//+car->_drvPos_z;

    osg::Matrix mat(car->_posMat[0][0],car->_posMat[0][1],car->_posMat[0][2],car->_posMat[0][3],
            car->_posMat[1][0],car->_posMat[1][1],car->_posMat[1][2],car->_posMat[1][3],
            car->_posMat[2][0],car->_posMat[2][1],car->_posMat[2][2],car->_posMat[2][3],
            car->_posMat[3][0],car->_posMat[3][1],car->_posMat[3][2],car->_posMat[3][3]);

    if (_wing3)
    {
        if ((wingangle > 0.0) && (wingangle < 10.0))
            this->pWing3->setSingleChildOn(0);
        else if ((wingangle > 10.0) && (wingangle < 35.0))
            this->pWing3->setSingleChildOn(1);
        else
            this->pWing3->setSingleChildOn(2);
    }

    if (_driver)
    {
        if((steerangle > 0.0f) && (steerangle < 0.03f))
            pDriver->setSingleChildOn(1);
        else if((steerangle > 0.03f) && (steerangle < 0.07f))
            pDriver->setSingleChildOn(2);
        else if((steerangle > 0.07f) && (steerangle < 0.13f))
            pDriver->setSingleChildOn(3);
        else if((steerangle > 0.13f) && (steerangle < 0.21f))
            pDriver->setSingleChildOn(4);
        else if((steerangle > 0.21f) && (steerangle < 0.30f))
            pDriver->setSingleChildOn(5);
        else if((steerangle > 0.13f) && (steerangle < 0.21f))
            pDriver->setSingleChildOn(6);
        else if((steerangle > 0.30f) && (steerangle < 0.45f))
            pDriver->setSingleChildOn(7);
        else if(steerangle > 0.45f)
            pDriver->setSingleChildOn(8);
        else if((steerangle < 0.0f) && (steerangle > -0.03f))
            pDriver->setSingleChildOn(9);
        else if((steerangle < 0.03f) && (steerangle > -0.07f))
            pDriver->setSingleChildOn(10);
        else if((steerangle < 0.07f) && (steerangle > -0.13f))
            pDriver->setSingleChildOn(11);
        else if((steerangle < 0.13f) && (steerangle > -0.21f))
            pDriver->setSingleChildOn(12);
        else if((steerangle < 0.21f) && (steerangle > -0.30f))
            pDriver->setSingleChildOn(13);
        else if((steerangle < 0.30f) && (steerangle > -0.45f))
            pDriver->setSingleChildOn(14);
        else if(steerangle < 0.45f)
            pDriver->setSingleChildOn(15);
        else
            pDriver->setSingleChildOn(0);
    }

    if(_steer)
    {
        steerangle = (-steerangle * 1.2);
        osg::ref_ptr<osg::MatrixTransform> movt = new osg::MatrixTransform;
        osg::Matrix rotation = osg::Matrix::rotate(steerangle, osg::X_AXIS);

        //osg::MatrixTransform * movt = dynamic_cast<osg::MatrixTransform *>(Steer_branch->getChild(0));
        movt->setMatrix(rotation);
        movt->addChild(pSteer);
        GfLogDebug(" # update steer branch\n");
    }

    for(std::vector<SDCarLight>::iterator i = lights.begin(); i != lights.end(); ++i)
        i->update(*this);

    wheels.updateWheels();

    this->car_branch->setMatrix(mat);
    this->lights_branch->setMatrix(mat);

    if(_carShader > 2)
        reflectionMapping->update();

    this->setReflectionMap(reflectionMapping->getReflectionMap());

    //ugly computation,
    /*if (SHADOW_TECHNIQUE == 0)
    {
        osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;
        for(unsigned int i=0;i<shadowVertices->size();i++)
        {
            osg::Vec3 vtx = (*shadowVertices.get())[i];
            osg::Vec4 vtx_world = osg::Vec4(vtx,1.0f)*mat;
            vtx_world._v[2] = RtTrackHeightG(car->_trkPos.seg, vtx_world.x(), vtx_world.y()); //0.01 needed, we have to sort out why
            vertices->push_back(osg::Vec3(vtx_world.x(), vtx_world.y(), vtx_world.z()));
        }
        quad->setVertexArray(vertices);
    }*/
}

void SDCar::updateShadingParameters(const osg::Matrixf &modelview)
{
    if (shader) shader->update(modelview);
}

void SDCar::setReflectionMap(osg::ref_ptr<osg::Texture> map)
{
    car_shaded_body->getOrCreateStateSet()->setTextureAttributeAndModes(2, map,
        osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
}

SDCars::SDCars(void) :
    cars_branch(NULL)
{

}

SDCars::~SDCars(void)
{
    for(unsigned i=0;i<the_cars.size();i++)
    {
        delete the_cars[i];
    }
}

void SDCars::addSDCar(SDCar *car)
{
    the_cars.insert(the_cars.end(), car);
}

void SDCars::loadCars(tSituation *pSituation, bool trackType, bool subCat)
{
    cars_branch = new osg::Group;
    SDRender *rend = (SDRender *)getRender();
    unsigned carShader = rend->getShader();
    tSituation *s = pSituation;
    this->situation = pSituation;

    for (int i = 0; i < s->_ncars; i++)
    {
        tCarElt* elt = s->cars[i];
        strncpy(elt->_masterModel, GfParmGetStr(elt->_carHandle, SECT_GROBJECTS, PRM_TEMPLATE, ""), MAX_NAME_LEN - 1);
        elt->_masterModel[MAX_NAME_LEN - 1] = 0;
        SDCar * car = new SDCar;
        this->addSDCar(car);
        this->cars_branch->addChild(car->loadCar(elt, trackType, subCat, carShader));
    }

    return;
}

SDCar *SDCars::getCar(tCarElt *car)
{
    std::vector<SDCar *>::iterator it;
    SDCar *res = new SDCar;

    for(it = the_cars.begin(); it!= the_cars.end(); ++it)
    {
        if((*it)->isCar(car))
        {
            res = *it;
        }
    }

    return res;
}

void SDCars::updateCars()
{
    std::vector<SDCar *>::iterator it;

    for(it = the_cars.begin(); it!= the_cars.end(); ++it)
    {
        (*it)->updateCar();
    }
}

void SDCars::markCarCurrent(tCarElt *car)
{
    std::vector<SDCar *>::iterator it;

    for(it = the_cars.begin(); it!= the_cars.end(); ++it)
    {
        (*it)->markCarCurrent(car);
    }
}

void SDCars::updateShadingParameters(const osg::Matrixf &modelview)
{
    std::vector<SDCar *>::iterator it;

    for(it = the_cars.begin(); it!= the_cars.end(); ++it)
    {
        (*it)->updateShadingParameters(modelview);
    }
}

void SDCars::unLoad()
{
    cars_branch->removeChildren(0, cars_branch->getNumChildren());
    cars_branch = NULL;
}
