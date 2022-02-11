/***************************************************************************

    file                 : OsgLightTransform.cpp
    created              : Tue Mar 31 20:41:16 CEST 2020
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


#include <osg/CullStack>

#include "OsgLightTransform.h"

static const double minScale = 1e-6;


SDLightTransform::SDLightTransform():
    exponent(1)
{
}


SDLightTransform::SDLightTransform(const SDLightTransform &pat, const osg::CopyOp &copyop):
    osg::Transform(pat, copyop),
    position(pat.position),
    matrix(pat.matrix),
    appliedMatrix(pat.matrix),
    exponent(pat.exponent)
{
}


bool SDLightTransform::computeLocalToWorldMatrix(osg::Matrix &matrix, osg::NodeVisitor *nv) const
{
    if (_referenceFrame == osg::Transform::RELATIVE_RF)
        matrix.preMult(appliedMatrix);
    else
        matrix = appliedMatrix;

    return true;
}


bool SDLightTransform::computeWorldToLocalMatrix(osg::Matrix &matrix, osg::NodeVisitor *nv) const
{
    osg::Matrix m;
    m.invert(appliedMatrix);
    if (_referenceFrame == osg::Transform::RELATIVE_RF)
        matrix.postMult(m);
    else
        matrix = m;

    return true;
}

void SDLightTransform::accept(osg::NodeVisitor &nv)
{
    if (nv.validNodeMask(*this))
    {
        if (nv.getVisitorType()==osg::NodeVisitor::CULL_VISITOR)
        {
            if (osg::CullStack* cs = dynamic_cast<osg::CullStack*>(&nv))
            {
                osg::Vec3d pos = cs->getEyeLocal() - position;
                pos.normalize();
                double scale = pos*normal;

                if (scale <= minScale)
                {
                    appliedMatrix = matrix
                                  * osg::Matrix::scale(minScale, minScale, minScale)
                                  * osg::Matrix::translate(position);
                }
                else
                {
                    const double *m = cs->getModelViewMatrix()->ptr();
                    scale = pow(scale, -fabs(exponent));
                    osg::Vec3d x(m[0], m[1], m[2]);
                    osg::Vec3d y(m[4], m[5], m[6]);
                    osg::Vec3d z(m[8], m[9], m[10]);
                    x.normalize();
                    y.normalize();
                    z.normalize();
                    x *= scale;
                    y *= scale;
                    z *= scale;
                    osg::Matrix rot(
                        x.x(), x.y(), x.z(), 0,
                        y.x(), y.y(), y.z(), 0,
                        z.x(), z.y(), z.z(), 0,
                        0, 0, 0, 1 );
                    osg::Matrix invRot;
                    invRot.invert_4x3(rot);
                    appliedMatrix = matrix * invRot * osg::Matrix::translate(position);
                }
            }
        }

        Transform::accept(nv);
    }
}
