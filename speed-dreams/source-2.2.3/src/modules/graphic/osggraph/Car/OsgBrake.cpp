/***************************************************************************

    file                 : OsgBrake.cpp
    created              : Mon Dec 31 10:24:02 CEST 2012
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

#include <osg/Geometry>
#include <osg/Geode>

#include <portability.h>

#include "OsgBrake.h"
#include "OsgMath.h"

void SDBrakes::setCar(tCarElt * car)
{
    this->car = car;
}

osg::Node *SDBrakes::initBrake(int wheelIndex)
{
    float	alpha;
    osg::Vec3 vtx;
    osg::Vec4 clr;
    osg::Vec2 tex;
    float	b_offset = 0;
    tdble	curAngle = 0.0;
    int i;

    tdble tireWidth = car->_tireWidth(wheelIndex);
    tdble brakeDiskRadius = car->_brakeDiskRadius(wheelIndex);

    osg::Geode *pBrake = new osg::Geode;
    pBrake->setName("Brake Assembly");
    pBrake->setCullingActive(false);
    osg::StateSet* stateSet = pBrake->getOrCreateStateSet();
    stateSet->setMode(GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED);
    stateSet->setMode(GL_CULL_FACE, osg::StateAttribute::OFF);
    stateSet->setMode(GL_BLEND, osg::StateAttribute::OFF);
    stateSet->setMode(GL_ALPHA_TEST, osg::StateAttribute::OFF);

    int brakeBranch = 32;
    float brakeAngle = 2.0 * SD_PI / (tdble)brakeBranch;
    float brakeOffset = 0.1;

    switch(wheelIndex)
    {
    case FRNT_RGT:
        curAngle = -(SD_PI / 2.0 + brakeAngle);
        b_offset = brakeOffset - tireWidth / 2.0;
        break;
    case FRNT_LFT:
        curAngle = -(SD_PI / 2.0 + brakeAngle);
        b_offset = tireWidth / 2.0 - brakeOffset;
        break;
    case REAR_RGT:
        curAngle = (SD_PI / 2.0 - brakeAngle);
        b_offset = brakeOffset - tireWidth / 2.0;
        break;
    case REAR_LFT:
        curAngle = (SD_PI / 2.0 - brakeAngle);
        b_offset = tireWidth / 2.0 - brakeOffset;
        break;
    }

    /* hub */
    osg::Vec3Array *hub_vtx = new osg::Vec3Array();
    osg::Vec4Array *hub_clr = new osg::Vec4Array();

    tdble hubRadius;

    /* center */
    vtx[0] = vtx[2] = 0.0;
    vtx[1] = b_offset;
    hub_vtx->push_back(vtx);

    hubRadius = brakeDiskRadius * 0.6;
    for (i = 0; i < brakeBranch; i++)
    {
        alpha = (float)i * 2.0 * SD_PI / (float)(brakeBranch - 1);
        vtx[0] = hubRadius * cos(alpha);
        vtx[1] = b_offset;
        vtx[2] = hubRadius * sin(alpha);
        hub_vtx->push_back(vtx);
    }

    clr[0] = clr[1] = clr[2] = 0.0;
    clr[3] = 1.0;
    hub_clr->push_back(clr);

    osg::Geometry *pHub = new osg::Geometry;
    pHub->setName("Hub");
    pHub->setVertexArray(hub_vtx);
    pHub->setColorArray(hub_clr, osg::Array::BIND_OVERALL);
    pHub->addPrimitiveSet( new osg::DrawArrays(osg::PrimitiveSet::TRIANGLE_FAN, 0, hub_vtx->size()) );

    pBrake->addDrawable(pHub);

    /* Brake disk */
    osg::Vec3Array *brk_vtx = new osg::Vec3Array();
    osg::Vec4Array *brk_clr = new osg::Vec4Array();

    for (i = 0; i < (brakeBranch / 2 + 2); i++)
    {
        alpha = curAngle + (float)i * 2.0 * M_PI / (float)(brakeBranch - 1);
        vtx[0] = brakeDiskRadius * cos(alpha);
        vtx[1] = b_offset;
        vtx[2] = brakeDiskRadius * sin(alpha);
        brk_vtx->push_back(vtx);
        vtx[0] = brakeDiskRadius * cos(alpha) * 0.6;
        vtx[1] = b_offset;
        vtx[2] = brakeDiskRadius * sin(alpha) * 0.6;
        brk_vtx->push_back(vtx);
    }

    clr[0] = clr[1] = clr[2] = 0.1;
    clr[3] = 1.0;
    brk_clr->push_back(clr);

    osg::Geometry *pBDisc = new osg::Geometry;
    pBDisc->setName("Disc Brake");
    pBDisc->setUseDisplayList(false);
    pBDisc->setVertexArray(brk_vtx);
    pBDisc->setColorArray(brk_clr, osg::Array::BIND_OVERALL);
    pBDisc->addPrimitiveSet( new osg::DrawArrays(osg::PrimitiveSet::TRIANGLE_STRIP, 0, brk_vtx->size()) );

    pBrake->addDrawable(pBDisc);
    brake_colors[wheelIndex] = brk_clr;

    /* Brake caliper */
    osg::Vec3Array *cal_vtx = new osg::Vec3Array();
    osg::Vec4Array *cal_clr = new osg::Vec4Array();

    for (i = 0; i < (brakeBranch / 2 - 2); i++)
    {
        alpha = - curAngle + (float)i * 2.0 * SD_PI / (float)(brakeBranch - 1);
        vtx[0] = (brakeDiskRadius + 0.02) * cos(alpha);
        vtx[1] = b_offset;
        vtx[2] = (brakeDiskRadius + 0.02) * sin(alpha);
        cal_vtx->push_back(vtx);
        vtx[0] = brakeDiskRadius * cos(alpha) * 0.6;
        vtx[1] = b_offset;
        vtx[2] = brakeDiskRadius * sin(alpha) * 0.6;
        cal_vtx->push_back(vtx);
    }

    clr[0] = clr[1] = clr[2] = 0.2;
    clr[3] = 1.0;
    cal_clr->push_back(clr);

    osg::Geometry *pCaliper = new osg::Geometry;
    pCaliper->setName("Caliper");
    pCaliper->setVertexArray(cal_vtx);
    pCaliper->setColorArray(cal_clr, osg::Array::BIND_OVERALL);
    pCaliper->addPrimitiveSet( new osg::DrawArrays(osg::PrimitiveSet::TRIANGLE_STRIP, 0, cal_vtx->size()) );

    pBrake->addDrawable(pCaliper);

    return pBrake;
}

void SDBrakes::updateBrakes()
{
    for(int i=0; i<4; i++)
    {
        osg::Vec4 clr;
        tdble temp = car->_brakeTemp(i);

        clr[0] = 0.1 + temp * 1.5;
        clr[1] = 0.1 + temp * 0.3;
        clr[2] = 0.1 - temp * 0.3;
        clr[3] = 1.0;
        (*brake_colors[i])[0] = clr;
        brake_colors[i]->dirty();

        //GfLogInfo("brake temp %i = %f\n", i, temp);
    }
}
