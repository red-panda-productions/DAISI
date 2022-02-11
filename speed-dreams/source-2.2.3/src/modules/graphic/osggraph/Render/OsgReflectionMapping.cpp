/***************************************************************************

    file        : OsgScreens.h
    created     : Sat Feb 2013 15:52:19 CEST 2013
    copyright   : (C) 2013 by Gaëtan André
    email       : gaetan.andre@gmail.com
    version     : $Id$
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ****************************************************************************/

#include <osg/Group>
#include <osg/Texture2D>
#include <osg/Camera>
#include <osgViewer/Viewer>

#include "OsgScreens.h"
#include "OsgCar.h"
#include "OsgRender.h"
#include "OsgMain.h"
#include "OsgReflectionMapping.h"
#include "OsgNodeMask.h"

#include <car.h>

SDReflectionMapping::SDReflectionMapping(SDCar *c):
    camerasRoot(NULL),
    reflectionMap(NULL),
    car(c)
{
    SDRender *render = (SDRender *)getRender();
    unsigned reflectionShader = render->getShader();
    osg::ref_ptr<osg::Node> m_sceneroot =  render->getRoot();

    osg::ref_ptr<osg::Image> imagePosX = new osg::Image;
    osg::ref_ptr<osg::Image> imageNegX = new osg::Image;
    osg::ref_ptr<osg::Image> imagePosY = new osg::Image;
    osg::ref_ptr<osg::Image> imageNegY = new osg::Image;
    osg::ref_ptr<osg::Image> imagePosZ = new osg::Image;
    osg::ref_ptr<osg::Image> imageNegZ = new osg::Image;

    imagePosX->setInternalTextureFormat(GL_RGB);
    imageNegX->setInternalTextureFormat(GL_RGB);
    imagePosY->setInternalTextureFormat(GL_RGB);
    imageNegY->setInternalTextureFormat(GL_RGB);
    imagePosZ->setInternalTextureFormat(GL_RGB);
    imageNegZ->setInternalTextureFormat(GL_RGB);

    osg::ref_ptr<osg::TextureCubeMap> reflectionmap = new osg::TextureCubeMap;

    this->reflectionMap = reflectionmap;

    reflectionmap->setImage(osg::TextureCubeMap::POSITIVE_X, imagePosX);
    reflectionmap->setImage(osg::TextureCubeMap::NEGATIVE_X, imageNegX);
    reflectionmap->setImage(osg::TextureCubeMap::POSITIVE_Y, imagePosY);
    reflectionmap->setImage(osg::TextureCubeMap::NEGATIVE_Y, imageNegY);
    reflectionmap->setImage(osg::TextureCubeMap::POSITIVE_Z, imagePosZ);
    reflectionmap->setImage(osg::TextureCubeMap::NEGATIVE_Z, imageNegZ);

    reflectionmap->setTextureSize( 256, 256 );
    reflectionmap->setInternalFormat( GL_RGB);
    reflectionmap->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE);
    reflectionmap->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_EDGE);
    reflectionmap->setWrap(osg::Texture::WRAP_R, osg::Texture::CLAMP_TO_EDGE);
    reflectionmap->setFilter(osg::TextureCubeMap::MIN_FILTER,osg::TextureCubeMap::LINEAR);
    reflectionmap->setFilter(osg::TextureCubeMap::MAG_FILTER,osg::TextureCubeMap::LINEAR);

    camerasRoot = new osg::Group;

    for(int i=0; i<6; i++)
    {
        osg::ref_ptr<osg::Camera> camera = new osg::Camera;
        camera->setViewport( 0, 0, 256, 256 );
        camera->setClearMask( GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT );
        //camera->setClearColor(osg::Vec4(0.0,0.0,0.0,1.0));

        camera->setRenderOrder( osg::Camera::PRE_RENDER );
        camera->setRenderTargetImplementation( osg::Camera::FRAME_BUFFER_OBJECT );
        camera->attach( osg::Camera::COLOR_BUFFER, reflectionMap, 0, i );
        camera->setReferenceFrame( osg::Camera::ABSOLUTE_RF );
        camera->addChild( m_sceneroot );
        camera->setProjectionMatrixAsPerspective(90.0, 1.0, 1.0, 80000.0);

        if (reflectionShader > 1)
            camera->setNodeMask(NODE_MASK_NONE);

        camerasRoot->addChild(camera);
        cameras.push_back(camera);


    }

    //ACTIVATE to enable Environment MApping <= temporary hack.
    if (reflectionShader > 1)
    {
        SDScreens * screens = (SDScreens*)getScreens();
        screens->registerViewDependantPreRenderNode(this->getCamerasRoot());
        GfLogDebug("Graphic options : reflectionShader = %u\n", reflectionShader);

        cameras[4]->setNodeMask(NODE_MASK_ALL);
    }
}

void SDReflectionMapping::update()
{
    //TODO support for multi screen
    SDScreens *screens = (SDScreens*)getScreens();
    osg::Camera * viewCam = screens->getActiveView()->getOsgCam();

    tCarElt *Car = this->car->getCar();

    sgVec3 p;
    osg::Vec3 eye, center, up;

    p[0] = Car->_drvPos_x;
    p[1] = Car->_bonnetPos_y;
    p[2] = Car->_drvPos_z;
    sgXformPnt3(p, Car->_posMat);

    eye[0] = p[0];
    eye[1] = p[1];
    eye[2] = p[2];

    osg::Matrix n = osg::Matrix(-1.0,0.0,0.0,0.0,
                                0.0,1.0,0.0,0.0,
                                0.0,0.0,1.0,0.0,
                                0.0,0.0,0.0,1.0);

    cameras[osg::TextureCubeMap::POSITIVE_Z]->setViewMatrix(osg::Matrix::translate(-eye)*osg::Matrix::rotate(viewCam->getViewMatrix().getRotate())*n);
    osg::Matrix mat = cameras[osg::TextureCubeMap::POSITIVE_Z]->getViewMatrix();

    osg::Matrix negX = osg::Matrix::rotate(osg::inDegrees(-90.0),osg::Y_AXIS);
    osg::Matrix negZ = osg::Matrix::rotate(osg::inDegrees(-180.0),osg::Y_AXIS);
    osg::Matrix posX = osg::Matrix::rotate(osg::inDegrees(90.0),osg::Y_AXIS);

    osg::Matrix negY= osg::Matrix::rotate(osg::inDegrees(-90.0),osg::X_AXIS);
    osg::Matrix posY= osg::Matrix::rotate(osg::inDegrees(90.0),osg::X_AXIS);

    cameras[osg::TextureCubeMap::NEGATIVE_X]->setViewMatrix(mat*negX);
    cameras[osg::TextureCubeMap::NEGATIVE_Z]->setViewMatrix(mat*negZ);
    cameras[osg::TextureCubeMap::POSITIVE_X]->setViewMatrix(mat*posX);

    cameras[osg::TextureCubeMap::NEGATIVE_Y]->setViewMatrix(mat*negY);
    cameras[osg::TextureCubeMap::POSITIVE_Y]->setViewMatrix(mat*posY);
}

SDReflectionMapping::~SDReflectionMapping()
{
}
