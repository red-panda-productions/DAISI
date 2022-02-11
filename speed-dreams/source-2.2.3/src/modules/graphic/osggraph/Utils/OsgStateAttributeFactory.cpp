/* -*-c++-*-
 *
 * Copyright (C) 2007 Tim Moore
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 *
 */
#include "OsgStateAttributeFactory.h"

#include <osg/AlphaFunc>
#include <osg/Array>
#include <osg/BlendFunc>
#include <osg/CullFace>
#include <osg/Depth>
#include <osg/ShadeModel>
#include <osg/Texture2D>
#include <osg/Texture3D>
#include <osg/TexEnv>

#include <osg/Image>

using namespace osg;

SDStateAttributeFactory::SDStateAttributeFactory()
{
    _standardAlphaFunc = new AlphaFunc;
    _standardAlphaFunc->setFunction(osg::AlphaFunc::GREATER);
    _standardAlphaFunc->setReferenceValue(0.01f);
    _standardAlphaFunc->setDataVariance(Object::STATIC);
    _smooth = new ShadeModel;
    _smooth->setMode(ShadeModel::SMOOTH);
    _smooth->setDataVariance(Object::STATIC);
    _flat = new ShadeModel(ShadeModel::FLAT);
    _flat->setDataVariance(Object::STATIC);
    _standardBlendFunc = new BlendFunc;
    _standardBlendFunc->setSource(BlendFunc::SRC_ALPHA);
    _standardBlendFunc->setDestination(BlendFunc::ONE_MINUS_SRC_ALPHA);
    _standardBlendFunc->setDataVariance(Object::STATIC);
    _standardTexEnv = new TexEnv;
    _standardTexEnv->setMode(TexEnv::MODULATE);
    _standardTexEnv->setDataVariance(Object::STATIC);
    osg::Image *dummyImage = new osg::Image;
    dummyImage->allocateImage(1, 1, 1, GL_LUMINANCE_ALPHA,
                              GL_UNSIGNED_BYTE);
    unsigned char* imageBytes = dummyImage->data(0, 0);
    imageBytes[0] = 255;
    imageBytes[1] = 255;
    _whiteTexture = new osg::Texture2D;
    _whiteTexture->setImage(dummyImage);
    _whiteTexture->setWrap(osg::Texture::WRAP_S, osg::Texture::REPEAT);
    _whiteTexture->setWrap(osg::Texture::WRAP_T, osg::Texture::REPEAT);
    _whiteTexture->setDataVariance(osg::Object::STATIC);
    // And now the transparent texture
    dummyImage = new osg::Image;
    dummyImage->allocateImage(1, 1, 1, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE);
    imageBytes = dummyImage->data(0, 0);
    imageBytes[0] = 255;
    imageBytes[1] = 0;
    _transparentTexture = new osg::Texture2D;
    _transparentTexture->setImage(dummyImage);
    _transparentTexture->setWrap(osg::Texture::WRAP_S, osg::Texture::REPEAT);
    _transparentTexture->setWrap(osg::Texture::WRAP_T, osg::Texture::REPEAT);
    _transparentTexture->setDataVariance(osg::Object::STATIC);
    _white = new Vec4Array(1);
    (*_white)[0].set(1.0f, 1.0f, 1.0f, 1.0f);
    _white->setDataVariance(Object::STATIC);
    _cullFaceFront = new CullFace(CullFace::FRONT);
    _cullFaceFront->setDataVariance(Object::STATIC);
    _cullFaceBack = new CullFace(CullFace::BACK);
    _cullFaceBack->setDataVariance(Object::STATIC);
    _depthWritesDisabled = new Depth(Depth::LESS, 0.0, 1.0, false);
    _depthWritesDisabled->setDataVariance(Object::STATIC);
}


// anchor the destructor into this file, to avoid ref_ptr warnings
SDStateAttributeFactory::~SDStateAttributeFactory()
{
}
