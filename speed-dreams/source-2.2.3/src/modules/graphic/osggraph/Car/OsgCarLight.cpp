/***************************************************************************

    file                 : OsgCarLight.cpp
    created              : Tue Mar 31 15:34:18 CEST 2020
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


#include <osg/PolygonOffset>
#include <osg/TexEnv>
#include <osg/Depth>
#include "OsgLoader.h"
#include "OsgLightTransform.h"
#include "OsgNodeMask.h"
#include "OsgCarLight.h"
#include "OsgCar.h"


osg::ref_ptr<osg::Node> SDCarLight::init(
    CarLightType type,
    osg::ref_ptr<osg::StateSet> state_set,
    const osg::Vec3d &position,
    const osg::Vec3d &normal,
    double size,
    int layers )
{
    if (layers < 0) layers = 0;
    
    this->type = type;
    
    osg::ref_ptr<SDLightTransform> transform = new SDLightTransform;
    transform->setPosition(position);
    transform->setNormal(normal);
    transform->setMatrix(osg::Matrix::scale(size, size, size));
    
    if (layers > 0) {
        const double s = 1;
        const double z = 0; // put 1 to move light bit close to camera
        
        const osg::Vec3d vtx[] = {
            osg::Vec3d(-s,-s, z),
            osg::Vec3d( s,-s, z),
            osg::Vec3d( s, s, z),
            osg::Vec3d(-s, s, z) };
        const osg::Vec2d tex[] = {
            osg::Vec2d(0, 0),
            osg::Vec2d(1, 0),
            osg::Vec2d(1, 1),
            osg::Vec2d(0, 1) };

        // build triangle strip
        const int count = (layers+1)*2;
        osg::Vec3dArray *vertexArray = new osg::Vec3dArray;
        osg::Vec2dArray *texArray = new osg::Vec2dArray;
        for(int l = 0; l < layers; ++l) {
            for(int i = 0; i < 4; ++i) {
                vertexArray->push_back(vtx[i]);
                texArray->push_back(tex[i]);
            }
        }

        osg::Vec4dArray *colorArray = new osg::Vec4dArray;
        colorArray->push_back( osg::Vec4d(0.8, 0.8, 0.8, 0.75) );

        osg::Vec3dArray *normalArray = new osg::Vec3dArray;
        normalArray->push_back( osg::Vec3d(0.0, 0.0, -1.0) );
        
        osg::Geometry *geometry = new osg::Geometry;
        geometry->setVertexArray(vertexArray);
        geometry->setTexCoordArray(0, texArray, osg::Array::BIND_PER_VERTEX);
        geometry->setNormalArray(normalArray, osg::Array::BIND_OVERALL);
        geometry->setColorArray(colorArray, osg::Array::BIND_OVERALL);
        geometry->addPrimitiveSet( new osg::DrawArrays(osg::PrimitiveSet::QUADS, 0, count) );

        osg::Geode *geode = new osg::Geode;
        geode->addDrawable(geometry);
        geode->setStateSet(state_set);
        transform->addChild(geode);
    }
    
    node = transform;
    return node;
}


void SDCarLight::update(const SDCar &car)
{
    const tCarElt *carElt = car.getCar();
    
    bool visible = false;
    switch (type) {
    case CAR_LIGHT_TYPE_BRAKE:
    case CAR_LIGHT_TYPE_BRAKE2:
        if (carElt->_brakeCmd>0 || carElt->_ebrakeCmd>0) 
            visible = true;
        break;
    case CAR_LIGHT_TYPE_FRONT:
        if (carElt->_lightCmd & RM_LIGHT_HEAD1) 
            visible = true;
        break;
    case CAR_LIGHT_TYPE_FRONT2:
        if (carElt->_lightCmd & RM_LIGHT_HEAD2) 
            visible = true;
        break;
    case CAR_LIGHT_TYPE_REAR:
        if ((carElt->_lightCmd & RM_LIGHT_HEAD1) ||
            (carElt->_lightCmd & RM_LIGHT_HEAD2))
            visible = true;
        break;
    case CAR_LIGHT_TYPE_REAR2:
        if ((carElt->_lightCmd & RM_LIGHT_HEAD1) ||
            (carElt->_lightCmd & RM_LIGHT_HEAD2))
            visible = true;
        break;
    default:
        break;
    }
    
    node->setNodeMask(visible ? NODE_MASK_ALL : NODE_MASK_NONE);
}


void SDCarLights::loadStates()
{
    const static char *filenames[CAR_LIGHT_TYPE_COUNT] = {
        NULL,
        "frontlight1.png",
        "frontlight2.png",
        "rearlight1.png",
        "rearlight2.png",
        "breaklight1.png",
        "breaklight2.png",
        NULL
    };
    
    osgLoader loader;
    loader.AddSearchPath("data/textures");
    loader.AddSearchPath("data/img");
    loader.AddSearchPath(".");
    
    for(int i = 0; i < CAR_LIGHT_TYPE_COUNT; ++i) {
        state_sets[i].release();
        
        if (!filenames[i])
            continue;
        
        osg::ref_ptr<osg::Image> image = loader.LoadImageFile(filenames[i]);
        if (!image) {
            GfLogError("Failed to load car lights texture: %s\n", filenames[i]);
            continue;
        }

        osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D;
        texture->setDataVariance(osg::Object::STATIC);
        texture->setWrap(osg::Texture2D::WRAP_S, osg::Texture2D::CLAMP_TO_EDGE);
        texture->setWrap(osg::Texture2D::WRAP_T, osg::Texture2D::CLAMP_TO_EDGE);
        texture->setImage(image);

        osg::ref_ptr<osg::StateSet> state_set = new osg::StateSet;
        state_set->setMode(GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED);
        state_set->setMode(GL_CULL_FACE, osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED);
        state_set->setMode(GL_BLEND, osg::StateAttribute::ON);
        state_set->setMode(GL_FOG, osg::StateAttribute::ON);
        state_set->setMode(GL_ALPHA_TEST, osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED);
        state_set->setTextureAttributeAndModes(0, texture, osg::StateAttribute::ON);
        state_set->setAttributeAndModes(new osg::PolygonOffset(-10000.f, -10000.f), osg::StateAttribute::ON);
        state_set->setTextureAttributeAndModes(0, new osg::TexEnv(osg::TexEnv::MODULATE), osg::StateAttribute::ON);
        state_set->setAttributeAndModes(new osg::Depth(osg::Depth::LESS, 0.0, 1.0, false), osg::StateAttribute::ON);
        state_set->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

        state_sets[i] = state_set;
    }
}
