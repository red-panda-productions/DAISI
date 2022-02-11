/***************************************************************************

    file                 : OsgLightTransform.h
    created              : Tue Mar 31 19:53:56 CEST 2020
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

#ifndef _OSGLIGHTTRANSFORM_H_
#define _OSGLIGHTTRANSFORM_H_

#include <osg/Transform>

class SDLightTransform: public osg::Transform
{
protected:
    osg::Vec3d position;
    osg::Vec3d normal;
    osg::Matrix matrix;
    osg::Matrix appliedMatrix;
    double exponent;

public :
    SDLightTransform();
    SDLightTransform(const SDLightTransform &pat, const osg::CopyOp &copyop = osg::CopyOp::SHALLOW_COPY);

protected :
    virtual ~SDLightTransform() {}

public :
    virtual osg::Object* cloneType() const { return new SDLightTransform(); }
    virtual osg::Object* clone(const osg::CopyOp &copyop) const { return new SDLightTransform(*this, copyop); }
    virtual bool isSameKindAs(const osg::Object *obj) const { return dynamic_cast<const SDLightTransform*>(obj)!=NULL; }
    virtual const char* className() const { return "SDLightTransform"; }
    virtual const char* libraryName() const { return "SD"; }

    virtual void accept(osg::NodeVisitor& nv);

    inline void setPosition(const osg::Vec3d &pos) { position = pos; }
    inline const osg::Vec3d& getPosition() const { return position; }

    inline void setNormal(const osg::Vec3d &n) { normal = n; normal.normalize(); }
    inline const osg::Vec3d& getNormal() const { return position; }

    inline void setMatrix(const osg::Matrix &m) { matrix = m; }
    inline const osg::Matrix& getMatrix() const { return matrix; }

    inline void setExponent(double e) { exponent = e; }
    inline double getExponent() const { return exponent; }

    virtual bool computeLocalToWorldMatrix(osg::Matrix &matrix, osg::NodeVisitor *nv) const;
    virtual bool computeWorldToLocalMatrix(osg::Matrix &matrix, osg::NodeVisitor *nv) const;
};

#endif
