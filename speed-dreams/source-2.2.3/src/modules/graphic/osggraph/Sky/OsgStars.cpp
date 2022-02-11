/***************************************************************************

    file                 : OsgStars.cpp
    created              : Mon Aug 21 18:24:02 CEST 2012
    copyright            : (C)2012 by Xavier Bertaux
    email                : bertauxx@yahoo.fr
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

#include <stdio.h>
#include <iostream>

#include <osg/AlphaFunc>
#include <osg/BlendFunc>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/Image>
#include <osg/Material>
#include <osg/Point>
#include <osg/ShadeModel>
#include <osg/Node>

#include <tgf.h>

//#include "OsgConstants.h"
#include "OsgStars.h"
#include "OsgMath.h"

// Constructor
SDStars::SDStars( void ) : old_phase(-1)
{
}

// Destructor
SDStars::~SDStars( void )
{
}

// initialize the stars object and connect it into our scene graph root
osg::Node* SDStars::build( int num, const osg::Vec3d star_data[], double star_dist )
{
    osg::Geode* geode = new osg::Geode;
    osg::StateSet* stateSet = geode->getOrCreateStateSet();
    stateSet->setRenderBinDetails(-9, "RenderBin");

    // set up the star state
    osg::BlendFunc* blendFunc = new osg::BlendFunc;
    blendFunc->setFunction(osg::BlendFunc::SRC_ALPHA,
                           osg::BlendFunc::ONE_MINUS_SRC_ALPHA);
    stateSet->setAttributeAndModes(blendFunc);

    stateSet->setMode(GL_FOG, osg::StateAttribute::OFF);
    stateSet->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
    stateSet->setMode(GL_CULL_FACE, osg::StateAttribute::OFF);
    stateSet->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
    stateSet->setMode(GL_ALPHA_TEST, osg::StateAttribute::OFF);

    stars_cl = new osg::Vec4Array;
    osg::Vec3Array* vl = new osg::Vec3Array;

    for ( int i = 0; i < num; ++i )
    {
        vl->push_back(osg::Vec3(star_dist * cos( star_data[i][0])
                                * cos( star_data[i][1] ),
                                star_dist * sin( star_data[i][0])
                                * cos( star_data[i][1] ),
                                star_dist * sin( star_data[i][1])));
        stars_cl->push_back(osg::Vec4(1, 1, 1, 0));
    }

    osg::Geometry* geometry = new osg::Geometry;
    geometry->setUseDisplayList(false);
    geometry->setVertexArray(vl);
    geometry->setColorArray(stars_cl.get());
    geometry->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
    geometry->setNormalBinding(osg::Geometry::BIND_OFF);
    geometry->addPrimitiveSet(new osg::DrawArrays(GL_POINTS, 0, vl->size()));
    geode->addDrawable(geometry);

    return geode;
}

/*bool cGrStars::reposition( osg::Vec3f& p, double angle )
{
  osg::Matrix T, SPIN;
  T.makeTranslate( p );
  SPIN.makeRotate(spin, osg::Vec3(0, 0, 1));

     star_transform->setMatrix( SPIN*T );

  return true;
}*/


bool SDStars::repaint( double sun_angle, int num, const osg::Vec3d star_data[] )
{
    double mag = 0.0, nmag = 0.0, alpha = 0.0, factor = 0.0, cutoff = 0.0;
    double mag_nakedeye = 7.6;
    double mag_twilight_astro = 6.4;
    double mag_twilight_nautic = 5.7;
    // sirius, brightest star (not brightest object)
    double mag_min = -1.46;

    int phase = 0;

    if ( sun_angle > (SD_PI_2 + 18.0 * SD_DEGREES_TO_RADIANS ) )
    {
        factor = 1.0;
        cutoff = mag_nakedeye;
        phase = 0;
        GfLogDebug(" Phase %i\n", phase);
    }
    else if ( sun_angle > (SD_PI_2 + 12.0 * SD_DEGREES_TO_RADIANS ) )
    {
        factor = 1.0;
        cutoff = mag_twilight_astro;
        phase = 1;
        GfLogDebug(" Phase %i\n", phase);
    }
    else if ( sun_angle > (SD_PI_2 + 9.0 * SD_DEGREES_TO_RADIANS ) )
    {
        factor = 1.0;
        cutoff = mag_twilight_nautic;
        phase = 2;
        GfLogDebug(" Phase %i\n", phase);
    }
    else if ( sun_angle > (SD_PI_2 + 7.5 * SD_DEGREES_TO_RADIANS ) )
    {
        factor = 0.95;
        cutoff = 4.1;
        phase = 3;
        GfLogDebug(" Phase %i\n", phase);
    }
    else if ( sun_angle > (SD_PI_2 + 7.0 * SD_DEGREES_TO_RADIANS ) )
    {
        factor = 0.9;
        cutoff = 3.4;
        phase = 4;
        GfLogDebug(" Phase %i\n", phase);
    }
    else if ( sun_angle > (SD_PI_2 + 6.5 * SD_DEGREES_TO_RADIANS ) )
    {
        factor = 0.85;
        cutoff = 2.8;
        phase = 5;
        GfLogDebug(" Phase %i\n", phase);
    }
    else if ( sun_angle > (SD_PI_2 + 6.0 * SD_DEGREES_TO_RADIANS ) )
    {
        factor = 0.8;
        cutoff = 1.2;
        phase = 6;
        GfLogDebug(" Phase %i\n", phase);
    }
    else if ( sun_angle > (SD_PI_2 + 5.5 * SD_DEGREES_TO_RADIANS ) )
    {
        factor = 0.75;
        cutoff = 0.6;
        phase = 7;
        GfLogDebug(" Phase %i\n", phase);
    }
    else
    {
        factor = 0.7;
        cutoff = 0.0;
        phase = 8;
        GfLogDebug(" Phase %i\n", phase);
    }

    if ( phase != old_phase )
    {
        old_phase = phase;

        for ( int i = 0; i < num; ++i )
        {
            mag = star_data[i][2];

            if ( mag < cutoff )
            {
                nmag = ( cutoff - mag ) / (cutoff - mag_min);
                alpha = nmag * 0.85 + 0.15;
                alpha *= factor;
            }
            else
            {
                alpha = 0.0;
            }

            if (alpha > 1.0) { alpha = 1.0; }
            if (alpha < 0.0) { alpha = 0.0; }

            GfLogDebug(" #stars mag = %.2f - nmag = %.2f - cutoff = %.2f - alpha = %.2f\n", mag, nmag, cutoff, alpha);

            (*stars_cl)[i] = osg::Vec4(1, 1, 1, alpha);
        }

        stars_cl->dirty();
    }
    else
    {
                //GfOut("  no phase change, skipping\n");
    }

    return true;
}
