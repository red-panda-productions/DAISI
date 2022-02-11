/***************************************************************************

    file                 : OsgBackground.cpp
    created              : Mon Aug 21 20:13:56 CEST 2012
    copyright            : (C) 2012 Xavier Bertaux
    email                : bertauxx@yahoo.fr
    version              : $Id: OsgBackground.cpp 2436 2010-05-08 14:22:43Z torcs-ng $

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
#include <osgDB/ReadFile>
#include <osgDB/WriteFile>
#include <osgDB/Registry>

#include "OsgScenery.h"

SDBackground::SDBackground(void) :
    _background(NULL),
	_backgroundTransform(NULL)
{
}

SDBackground::~SDBackground(void)
{
    if(_background != NULL)
    {
        _background->removeChildren(0, _background->getNumChildren());
        _background = NULL;
    }
}

void SDBackground::build(bool type, int X, int Y, int Z, const std::string& TrackPath)
{
    bool land = type;
    osgDB::Registry::instance()->clearObjectCache();

    std::string LocalPath = GetDataDir();

    osgDB::FilePathList pathList = osgDB::Registry::instance()->getDataFilePathList();

    pathList.push_front(LocalPath+"data/objects/");
    pathList.push_front(LocalPath+"data/textures/");
    pathList.push_front(TrackPath);
    osgDB::Registry::instance()->setDataFilePathList(pathList);

    //osg::ref_ptr<osg::MatrixTransform> _background_transform = new osg::MatrixTransform;
    osg::Matrix mat( 1.0f,  0.0f, 0.0f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f,
                     0.0f, -1.0f, 0.0f, 0.0f, 0.0f,  0.0f, 0.0f, 1.0f);

	_backgroundTransform = new osg::MatrixTransform;

    if (!land)
    {
        osg::ref_ptr<osg::Node> m_background = osgDB::readNodeFile("background-sky.ac");
        //_background_transform->setMatrix(mat);
        osg::Matrix t = osg::Matrix::translate(grWrldX /2, grWrldY /2, grWrldZ /2);
        mat = mat * t;
        _backgroundTransform->setMatrix(mat);
        _backgroundTransform->addChild( m_background.get() );
    }
    else
    {
        osg::ref_ptr<osg::Node> m_background = osgDB::readNodeFile("land.ac");
        _backgroundTransform->setMatrix(mat);
        _backgroundTransform->addChild( m_background.get() );
    }

    osgDB::Registry::instance()->setDataFilePathList( osgDB::FilePathList() );

    osg::ref_ptr<osg::StateSet> bgstate = _backgroundTransform->getOrCreateStateSet();
    bgstate->setRenderBinDetails(-1, "RenderBin");
    bgstate->setMode(GL_LIGHTING, osg::StateAttribute::ON);
    bgstate->setMode(GL_FOG, osg::StateAttribute::ON);

    _background = new osg::Group;
    _background->addChild(_backgroundTransform.get());
}

void SDBackground::reposition(double X, double Y, double Z)
{
	osg::Matrix T;
	osg::Matrix mat(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);

	T.makeTranslate(X, Y, Z);
   if(_backgroundTransform)
	   _backgroundTransform->setMatrix(mat * T);
}
