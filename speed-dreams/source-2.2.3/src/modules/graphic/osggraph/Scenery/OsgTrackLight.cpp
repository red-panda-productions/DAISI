/***************************************************************************

    file                 : OsgTrackLight.cpp
    created              : Sun Oct 05 20:13:56 CEST 2014
    copyright            : (C) 2014 Xavier Bertaux
    email                : bertauxx@yahoo.fr
    version              : $Id: OsgTrackLight.cpp 2436 2014-10-05 20:22:43Z torcs-ng $

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
#include <osg/Texture2D>
#include <osgDB/ReadFile>

#include "OsgScenery.h"


class SDTrackLights::Internal
{
public:
    struct Light
    {
        int index;
        osg::ref_ptr<osg::Geode> node;
        osg::ref_ptr<osg::StateSet> states[3];
        Light(): index() { }
        void setState(int index);
    };

    typedef std::vector<Light> LightList;

    int  onoff_red_index;
    bool onoff_red;
    bool onoff_green;
    bool onoff_green_st;
    bool onoff_yellow;
    bool onoff_phase;

    LightList red;
    LightList green;
    LightList green_st;
    LightList yellow;

    Internal():
        onoff_red_index(-1),
        onoff_red(),
        onoff_green(),
        onoff_green_st(),
        onoff_yellow(),
        onoff_phase()
    { }

    ~Internal() { }

    osg::ref_ptr<osg::StateSet> initStateSet(char const *textureFilename);
    void addLight(const osg::ref_ptr<osg::Group> &group, tGraphicLightInfo *info);
    void update(double currentTime, double totTime, int raceType);
};

osg::ref_ptr<osg::StateSet>
SDTrackLights::Internal::initStateSet(char const *textureFilename)
{
    osg::ref_ptr<osg::Image> image = osgDB::readImageFile(textureFilename);

    if (!image)
        GfLogError("Failed to load track lights texture: %s\n", textureFilename);

    osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D;
    texture->setDataVariance(osg::Object::STATIC);
    texture->setWrap(osg::Texture2D::WRAP_S, osg::Texture2D::CLAMP_TO_EDGE);
    texture->setWrap(osg::Texture2D::WRAP_T, osg::Texture2D::CLAMP_TO_EDGE);
    texture->setMaxAnisotropy(16);
    texture->setImage(image);

    osg::ref_ptr<osg::StateSet> stateSet = new osg::StateSet;
    stateSet->setMode(GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED);
    stateSet->setMode(GL_CULL_FACE, osg::StateAttribute::OFF);
    stateSet->setMode(GL_BLEND, osg::StateAttribute::ON);
    stateSet->setMode(GL_ALPHA_TEST, osg::StateAttribute::OFF);
    stateSet->setTextureAttributeAndModes(0, texture, osg::StateAttribute::ON);

    return stateSet;
}

void
SDTrackLights::Internal::addLight(const osg::ref_ptr<osg::Group> &group, tGraphicLightInfo *info)
{
    Light *new_light = 0;

    switch( info->role )
    {
    case GR_TRACKLIGHT_START_RED:
        red.push_back(Light());
        new_light = &red.back();
        break;
    case GR_TRACKLIGHT_START_GREEN:
        green.push_back(Light());
        new_light = &green.back();
        break;
    case GR_TRACKLIGHT_START_GREENSTART:
        green_st.push_back(Light());
        new_light = &green_st.back();
        break;
    case GR_TRACKLIGHT_START_YELLOW:
        yellow.push_back(Light());
        new_light = &yellow.back();
        break;
    case GR_TRACKLIGHT_POST_YELLOW:
    case GR_TRACKLIGHT_POST_GREEN:
    case GR_TRACKLIGHT_POST_RED:
    case GR_TRACKLIGHT_POST_BLUE:
    case GR_TRACKLIGHT_POST_WHITE:
    case GR_TRACKLIGHT_PIT_RED:
    case GR_TRACKLIGHT_PIT_GREEN:
    case GR_TRACKLIGHT_PIT_BLUE:
    default:
        break;
    }

    if (!new_light)
        return;
    
    int states = 2;

    osg::Vec3Array *vertexArray = new osg::Vec3Array;
    osg::Vec3Array *normalArray = new osg::Vec3Array;
    osg::Vec4Array *colourArray = new osg::Vec4Array;
    osg::Vec2Array *texArray    = new osg::Vec2Array;

    osg::Vec3 vertex;
    osg::Vec3 normal;
    osg::Vec4 colour;
    osg::Vec2 texcoord;

    colour[ 0 ] = info->red;
    colour[ 1 ] = info->green;
    colour[ 2 ] = info->blue;
    colour[ 3 ] = 1.0f;
    colourArray->push_back( colour );

    vertex[ 0 ] = info->topleft.x;
    vertex[ 1 ] = info->topleft.y;
    vertex[ 2 ] = info->topleft.z;
    vertexArray->push_back( vertex );
    vertex[ 2 ] = info->bottomright.z;
    vertexArray->push_back( vertex );
    vertex[ 0 ] = info->bottomright.x;
    vertex[ 1 ] = info->bottomright.y;
    vertex[ 2 ] = info->topleft.z; //?
    vertexArray->push_back( vertex );
    vertex[ 2 ] = info->topleft.z;
    vertex[ 2 ] = info->bottomright.z; //?
    vertexArray->push_back( vertex );

    normal[ 0 ] = info->bottomright.y - info->topleft.y;
    normal[ 1 ] = info->topleft.x - info->bottomright.x;
    normal[ 2 ] = 0.0;
    normal.normalize();
    normalArray->push_back( normal );

    texcoord[ 0 ] = 0.0;
    texcoord[ 1 ] = 0.0;
    texArray->push_back( texcoord );
    texcoord[ 0 ] = 0.0;
    texcoord[ 1 ] = 1.0;
    texArray->push_back( texcoord );
    texcoord[ 0 ] = 1.0;
    texcoord[ 1 ] = 0.0;
    texArray->push_back( texcoord );
    texcoord[ 0 ] = 1.0;
    texcoord[ 1 ] = 1.0;
    texArray->push_back( texcoord );

    if( info->role == GR_TRACKLIGHT_START_YELLOW || info->role == GR_TRACKLIGHT_POST_YELLOW ||
        info->role == GR_TRACKLIGHT_POST_GREEN || info->role == GR_TRACKLIGHT_POST_RED ||
        info->role == GR_TRACKLIGHT_POST_BLUE || info->role == GR_TRACKLIGHT_POST_WHITE ||
        info->role == GR_TRACKLIGHT_PIT_BLUE )
    {
        states = 3;
    }

    osg::Geometry *geometry = new osg::Geometry;
    geometry->setVertexArray(vertexArray);
    geometry->setTexCoordArray(0, texArray, osg::Array::BIND_PER_VERTEX);
    geometry->setNormalArray(normalArray, osg::Array::BIND_OVERALL);
    geometry->setColorArray(colourArray, osg::Array::BIND_OVERALL);
    geometry->addPrimitiveSet( new osg::DrawArrays(osg::PrimitiveSet::TRIANGLE_STRIP, 0, vertexArray->size()) );


    osg::ref_ptr<osg::StateSet> onState = initStateSet(info->onTexture);
    osg::ref_ptr<osg::StateSet> offState= initStateSet(info->offTexture);

    new_light->index = info->index;
    if (states == 3 && info->index % 2)
    {
        new_light->states[0] = offState;
        new_light->states[1] = offState;
        new_light->states[2] = onState;
    }
    else
    {
        new_light->states[0] = offState;
        new_light->states[1] = onState;
        new_light->states[2] = offState;
    }

    new_light->node = new osg::Geode;
    new_light->node->addDrawable(geometry);

    new_light->node->setStateSet( new_light->states[0] );

    group->addChild( new_light->node );
}

void
SDTrackLights::Internal::update(double currentTime, double totTime, int raceType)
{
    bool onoff;
    int current_index;

    bool active = currentTime >= 0.0f && ( totTime < 0.0f || currentTime < totTime );
    bool phase = (int)floor( fmod( currentTime + 120.0f, (double)0.3f ) / 0.3f ) % 2 == 1;

    if( currentTime < 0.0f )
        current_index = (int)floor( currentTime * (-10.0f) );
    else
        current_index = -1;

    onoff = !active && raceType != RM_TYPE_RACE;
    if( current_index != onoff_red_index || onoff != onoff_red )
    {
        onoff_red_index = current_index;
        onoff_red = onoff;
        for(LightList::iterator i = red.begin(); i != red.end(); ++i)
        {
            int index = onoff || (current_index >= 0 && current_index < i->index) ? 1 : 0;
            i->node->setStateSet( i->states[index] );
        }
    }

    onoff = active && raceType != RM_TYPE_RACE;
    if( onoff_green != onoff )
    {
        onoff_green = onoff;
        for(LightList::iterator i = green.begin(); i != green.end(); ++i)
            i->node->setStateSet( i->states[onoff ? 1 : 0] );
    }

    onoff = active && ( raceType != RM_TYPE_RACE || currentTime < 30.0f );
    if( onoff_green_st != onoff )
    {
        onoff_green_st = onoff;
        for(LightList::iterator i = green_st.begin(); i != green_st.end(); ++i)
            i->node->setStateSet( i->states[onoff ? 1 : 0] );
    }

    onoff = false;
    if( onoff_yellow != onoff || ( onoff && phase != onoff_phase ) )
    {
        onoff_yellow = onoff;
        int index = !onoff ? 0 : (phase ? 2 : 1);
        for(LightList::iterator i = yellow.begin(); i != yellow.end(); ++i)
            i->node->setStateSet( i->states[index] );
    }

    onoff_phase = phase;
}

SDTrackLights::SDTrackLights(void):
    internal()
{
}

SDTrackLights::~SDTrackLights(void)
{
    if (internal) delete internal;
}

void SDTrackLights::build(tTrack *track)
{
    if (internal) delete internal;

    internal = new Internal;
    _osgtracklight = new osg::Group;

    for(int i = 0; i < track->graphic.nb_lights; ++i)
        internal->addLight( _osgtracklight, &track->graphic.lights[i] );
}

void SDTrackLights::update(double currentTime, double totTime, int raceType)
{
    if (internal) internal->update(currentTime, totTime, raceType);
}
