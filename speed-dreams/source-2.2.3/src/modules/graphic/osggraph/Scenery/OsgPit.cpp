/***************************************************************************

    file                 : OsgPit.cpp
    created              : Sun Sep 07 20:13:56 CEST 2014
    copyright            : (C)2014 Xavier Bertaux
    email                : bertauxx@yahoo.fr
    version              : $Id: OsgPit.cpp 2436 2014-09-07 14:22:43Z torcs-ng $

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

#include "OsgScenery.h"
#include "OsgLoader.h"
#include <robottools.h> // RtXXX
#include <portability.h> // snprintf


SDPit::SDPit(void)
{
}

SDPit::~SDPit(void)
{
}

void SDPit::build(tTrack *track)
{
    GfLogDebug("Build pits ... \n");

    tTrackPitInfo *pits = &track->pits;

    if (pits->pitindicator > 0)
        GfLogTrace("Pit Indicator ... YES ...\n");
    else
        GfLogTrace("Pit Indicator ... NO ...\n");

    std::string localDir = GfLocalDir();
    pit_root = new osg::Group();

    // Create the pit identification on the wall if any.
    switch (pits->type)
    {
    case TR_PIT_ON_TRACK_SIDE:
        GfLogTrace("Creating track side pit buildings (%d slots) ...\n", pits->nMaxPits);
        for (int i = 0; i < pits->nMaxPits; i++)
        {
            tTrackOwnPit *act_pit = &pits->driversPits[i];

            osgLoader loader;

            // Default driver logo file name (pit door).
            std::string strLogoFileName("logo");

            // If we have at least one car in the pit, use the team pit logo of driver 0.
            osg::ref_ptr<osg::Image> image;
            if (tCarElt *car = act_pit->car[0])
            {
                char drvIndex[30];
                snprintf(drvIndex, sizeof(drvIndex), "%d", car->_driverIndex);
                loader.AddSearchPath(localDir + "drivers/" + car->_modName + "/" + drvIndex);
                loader.AddSearchPath(localDir + "drivers/" + car->_modName);
                loader.AddSearchPath(std::string("drivers/") + car->_modName + "/" + drvIndex);
                loader.AddSearchPath(std::string("drivers/") + car->_modName);

                // If a custom skin was selected, and it can apply to the pit door,
                // update the logo file name accordingly.
                if (strlen(car->_skinName) != 0
                        && car->_skinTargets & RM_CAR_SKIN_TARGET_PIT_DOOR)
                {
                    strLogoFileName += '-';
                    strLogoFileName += car->_skinName;
                    GfLogTrace("Using skinned pit door logo %s\n", strLogoFileName.c_str());
                }

                // Load logo texture (only rgbs - pngs cause pit transparency bug # 387)
                image = loader.LoadImageFile(strLogoFileName + ".png");
                if (!image) image = loader.LoadImageFile(strLogoFileName + ".rgb");
            }  // if car

            // If no car in the pit, or logo file not found, hope for the .rgb in data/textures.
            if (!image) {
                loader.AddSearchPath("data/textures");
                image = loader.LoadImageFile(strLogoFileName + ".png");
                if (!image) image = loader.LoadImageFile(strLogoFileName + ".rgb");
            }

            // Pit wall texturing : the loaded 'logo*.rgb/.png' image file is supposed to consist
            // of 4 distinct parts :
            //
            //  **************************************************** 1.0
            //  *..................................................*
            //  *.                                                .*
            //  *.           Pit door (dots included)             .*
            //  *.                                                .*
            //  *.                                                .*
            //  *..................................................*
            //  ##########################$$$$$$$$$$$$$$$$$$$$$$$$$$ 0.33
            //  #                        #$                        $
            //  #      Team logo         #$     Team name          $
            //  #                        #$                        $
            //  ##########################$$$$$$$$$$$$$$$$$$$$$$$$$$ 0.0
            //  0.0                     0.5                      1.0
            //
            // - the stars '*' : Left, top and right 1-pixel-wide line :
            //   the pit wall (will be repeated respectively in the left, top and right
            //   direction in order to cover the pit-wall outside of the rectangle of the pit-door
            // - the dots '.' : the real pit door texture (actually includes the stars part)
            // - the hashes '#' : the team name texture
            // - the dollars '$' : the team logo texture
            //
            // More details here : http://www.berniw.org/torcs/robot/ch6/pitlogo.html

            osg::Vec3Array *pit_vtx = new osg::Vec3Array;
            osg::Vec2Array *pit_tex = new osg::Vec2Array;
            osg::Vec4Array *pit_clr = new osg::Vec4Array;
            osg::Vec3Array *pit_nrm = new osg::Vec3Array;

            pit_clr->push_back(osg::Vec4(0, 0, 0, 1));

            // Determine the position of the pit wall, and its normal vector.
            tdble x, y;
            t3Dd normalvector;
            RtTrackLocal2Global(&(act_pit->pos), &x, &y, act_pit->pos.type);
            RtTrackSideNormalG(act_pit->pos.seg, x, y, pits->side, &normalvector);

            // Normal
            pit_nrm->push_back(osg::Vec3(normalvector.x, normalvector.y, 0));

            // Determine the position of the first, bottom vertex of the triangle strip
            tdble x2 = x - pits->width / 2.0 * normalvector.x
                    + pits->len / 2.0 * normalvector.y;
            tdble y2 = y - pits->width / 2.0 * normalvector.y
                    - pits->len / 2.0 * normalvector.x;
            tdble z2 = RtTrackHeightG(act_pit->pos.seg, x2, y2);

            // First, bottom vertex of the triangle strip
            pit_vtx->push_back(osg::Vec3(x2, y2, z2));
            pit_tex->push_back(osg::Vec2(-0.7, 42.0/128));

            // First, top vertex of the triangle strip
            pit_vtx->push_back(osg::Vec3(x2, y2, z2 + 4.8));
            pit_tex->push_back(osg::Vec2(-0.7, 1.1));

            // Determine the position of the second, bottom vertex of the triangle strip
            x2 -= pits->len * normalvector.y;
            y2 += pits->len * normalvector.x;
            z2 = RtTrackHeightG(act_pit->pos.seg, x2, y2);

            // Second, bottom vertex of the triangle strip
            pit_vtx->push_back(osg::Vec3(x2, y2, z2));
            pit_tex->push_back(osg::Vec2(-0.7 + 2.0, 42.0/128));

            // Second, top vertex of the triangle strip
            pit_vtx->push_back(osg::Vec3(x2, y2, z2 + 4.8));
            pit_tex->push_back(osg::Vec2(-0.7 + 2.0, 1.1));

            // Build-up the vertex array.
            osg::Geometry *geometry = new osg::Geometry;
            geometry->setVertexArray(pit_vtx);
            geometry->setTexCoordArray(0, pit_tex, osg::Array::BIND_PER_VERTEX);
            geometry->setNormalArray(pit_nrm, osg::Array::BIND_OVERALL);
            geometry->setColorArray(pit_clr, osg::Array::BIND_OVERALL);
            geometry->addPrimitiveSet( new osg::DrawArrays(osg::PrimitiveSet::TRIANGLE_STRIP, 0, pit_vtx->size()) );

            osg::Geode *pit = new osg::Geode;
            pit->addDrawable(geometry);

            osg::StateSet *stateSet = pit->getOrCreateStateSet();
            stateSet->setMode(GL_CULL_FACE, osg::StateAttribute::OFF);
            stateSet->setMode(GL_BLEND, osg::StateAttribute::OFF);

            // Attach the texture to it.
            if (image) {
                osg::Texture2D *texture = new osg::Texture2D;
                texture->setDataVariance(osg::Object::STATIC);
                texture->setWrap(osg::Texture2D::WRAP_S, osg::Texture2D::CLAMP_TO_EDGE);
                texture->setWrap(osg::Texture2D::WRAP_T, osg::Texture2D::CLAMP_TO_EDGE);
                texture->setMaxAnisotropy(16);
                texture->setImage(image);
                stateSet->setTextureAttributeAndModes(0, texture, osg::StateAttribute::ON);
            }

            // Done.
            pit_root->addChild(pit);
        }  // for i
        break;

    case TR_PIT_NO_BUILDING:
    {
        GfLogTrace("Creating low pit wall (%d slots) ...\n", pits->nMaxPits);
        // This mode draws a white, low wall (about 3ft high) with logos

        // First we load the low wall's texture, as it is the same
        // for all the drivers
        osgLoader loaderAll;
        loaderAll.AddSearchPath(localDir + "data/textures");
        loaderAll.AddSearchPath("data/textures");
        osg::Texture2D *wallTexture = NULL;
        if (osg::ref_ptr<osg::Image> image = loaderAll.LoadImageFile("tr-wall-gr.png")) {
            wallTexture = new osg::Texture2D;
            wallTexture->setDataVariance(osg::Object::STATIC);
            wallTexture->setWrap(osg::Texture2D::WRAP_S, osg::Texture2D::CLAMP_TO_EDGE);
            wallTexture->setWrap(osg::Texture2D::WRAP_T, osg::Texture2D::CLAMP_TO_EDGE);
            wallTexture->setMaxAnisotropy(16);
            wallTexture->setImage(image);
        }

        bool bHasLogo = false;
        bool bHasPitIndicator = false;

        // Loop over all the available pits
        for (int i = 0; i < pits->nMaxPits; i++)
        {
            tTrackOwnPit *act_pit = &pits->driversPits[i];

            osgLoader loader;

            // Load this pit's own logo
            // Default driver logo filename (pit door)
            std::string strLogoFileName("logo");
            if (tCarElt *car = act_pit->car[0])
            {
                // If we have more than one car in the pit,
                // use the team pit logo of driver 0.

                char drvIndex[30];
                snprintf(drvIndex, sizeof(drvIndex), "%d", car->_driverIndex);
                loader.AddSearchPath(localDir + "drivers/" + car->_modName + "/" + drvIndex);
                loader.AddSearchPath(localDir + "drivers/" + car->_modName);
                loader.AddSearchPath(std::string("drivers/") + car->_modName + "/" + drvIndex);
                loader.AddSearchPath(std::string("drivers/") + car->_modName);

                // If a custom skin was selected, and it can apply to the pit door,
                // update the logo file name accordingly
                if (strlen(car->_skinName) != 0
                        && car->_skinTargets & RM_CAR_SKIN_TARGET_PIT_DOOR)
                {
                    strLogoFileName += '-';
                    strLogoFileName += car->_skinName;
                    GfLogTrace("Using skinned pit door logo %s\n",
                               strLogoFileName.c_str());
                }

                bHasLogo = true;
                if (pits->pitindicator > 0 && bHasLogo)
                    bHasPitIndicator = true;
                else
                    bHasPitIndicator = false;

            }
            else
            {
                bHasLogo = false;
                bHasPitIndicator = false;
            }  // if act_pit->car[0]

            // Get this pit's center coords
            tdble x0, y0;
            t3Dd normalvector;
            RtTrackLocal2Global(&(act_pit->pos), &x0, &y0, act_pit->pos.type);
            RtTrackSideNormalG(act_pit->pos.seg, x0, y0, pits->side, &normalvector);
            // This offset needed so the pit walls start at the correct place
            x0 = x0 - pits->width / 2.0 * normalvector.x
                    - pits->len / 4.0 * normalvector.y;

            // Let's draw the low wall
            // It is drawn in 3 parts.
            // First some small wall, then the logo, then some more wall.
            // Small wall bounds: (x1, y1) - (x2, y2)
            // Logo part bounds:  (x2, y2) - (x3, y3)
            // Small wall bounds: (x3, y3) - (x4, y4)

            tdble x1 = x0 - pits->width / 2.0 * normalvector.x
                    + pits->len / 2.0 * normalvector.y;
            tdble y1 = y0 - pits->width / 2.0 * normalvector.y
                    - pits->len / 2.0 * normalvector.x;
            tdble z1 = RtTrackHeightG(act_pit->pos.seg, x1, y1);

            tdble x2 = x0 - pits->width / 2.0 * normalvector.x
                    + pits->len / 4.0 * normalvector.y;
            tdble y2 = y0 - pits->width / 2.0 * normalvector.y
                    - pits->len / 4.0 * normalvector.x;
            tdble z2 = RtTrackHeightG(act_pit->pos.seg, x2, y2);

            tdble x3 = x0 - pits->width / 2.0 * normalvector.x
                    - pits->len / 4.0 * normalvector.y;
            tdble y3 = y0 - pits->width / 2.0 * normalvector.y
                    + pits->len / 4.0 * normalvector.x;
            tdble z3 = RtTrackHeightG(act_pit->pos.seg, x3, y3);

            tdble x4 = x0 - pits->width / 2.0 * normalvector.x
                    - pits->len / 2.0 * normalvector.y;
            tdble y4 = y0 - pits->width / 2.0 * normalvector.y
                    + pits->len / 2.0 * normalvector.x;
            tdble z4 = RtTrackHeightG(act_pit->pos.seg, x4, y4);

            osg::Vec4Array *pit_clr = new osg::Vec4Array;
            osg::Vec3Array *pit_nrm = new osg::Vec3Array;
            pit_clr->push_back(osg::Vec4(1, 1, 1, 1));
            pit_nrm->push_back(osg::Vec3(normalvector.x, normalvector.y, 0.0));

            { // wall
                osg::Vec3Array *pit_vtx = new osg::Vec3Array;
                osg::Vec2Array *pit_tex = new osg::Vec2Array;

                // part one, verticies from 0 to 3
                // First, bottom vertex
                pit_vtx->push_back(osg::Vec3(x1, y1, z1));
                pit_tex->push_back(osg::Vec2(0, 0));
                // First, top vertex
                pit_vtx->push_back(osg::Vec3(x1, y1, z1 + 0.9));
                pit_tex->push_back(osg::Vec2(0.0, 0.25));
                // Second, bottom vertex
                pit_vtx->push_back(osg::Vec3(x2, y2, z2));
                pit_tex->push_back(osg::Vec2(1.0, 0.0));
                // Second, top vertex
                pit_vtx->push_back(osg::Vec3(x2, y2, z2 + 0.9));
                pit_tex->push_back(osg::Vec2(1.0, 0.25));

                // part two, verticies from 4 to 7
                // First, bottom vertex
                pit_vtx->push_back(osg::Vec3(x3, y3, z3));
                pit_tex->push_back(osg::Vec2(0, 0));
                // First, top vertex
                pit_vtx->push_back(osg::Vec3(x3, y3, z3 + 0.9));
                pit_tex->push_back(osg::Vec2(0.0, 0.25));
                // Second, bottom vertex
                pit_vtx->push_back(osg::Vec3(x4, y4, z4));
                pit_tex->push_back(osg::Vec2(1.0, 0.0));
                // Second, top vertex
                pit_vtx->push_back(osg::Vec3(x4, y4, z4 + 0.9));
                pit_tex->push_back(osg::Vec2(1.0, 0.25));

                osg::Geometry *geometry = new osg::Geometry;
                geometry->setVertexArray(pit_vtx);
                geometry->setTexCoordArray(0, pit_tex, osg::Array::BIND_PER_VERTEX);
                geometry->setNormalArray(pit_nrm, osg::Array::BIND_OVERALL);
                geometry->setColorArray(pit_clr, osg::Array::BIND_OVERALL);
                geometry->addPrimitiveSet( new osg::DrawArrays(osg::PrimitiveSet::TRIANGLE_STRIP, 0, 4) );
                geometry->addPrimitiveSet( new osg::DrawArrays(osg::PrimitiveSet::TRIANGLE_STRIP, 4, 4) );

                osg::Geode *wall = new osg::Geode;
                wall->addDrawable(geometry);

                osg::StateSet *stateSet = wall->getOrCreateStateSet();
                stateSet->setMode(GL_CULL_FACE, osg::StateAttribute::OFF);
                stateSet->setMode(GL_BLEND, osg::StateAttribute::OFF);
                if (wallTexture)
                    stateSet->setTextureAttributeAndModes(0, wallTexture, osg::StateAttribute::ON);

                pit_root->addChild(wall);
            }

            { // logo
                osg::Vec3Array *pit_vtx = new osg::Vec3Array;
                osg::Vec2Array *pit_tex = new osg::Vec2Array;

                // If bHasLogo is false, there is no team/driver logo,
                // we should display a plain concrete wall then without decals.
                // In that case strLogoFileName == strWallFileName,
                // but we also must care for different coord mapping.

                // First, bottom vertex
                pit_vtx->push_back(osg::Vec3(x2, y2, z2));
                pit_tex->push_back(osg::Vec2(0.0, 0.0));
                // First, top vertex
                pit_vtx->push_back(osg::Vec3(x2, y2, z2 + 0.9));
                pit_tex->push_back(osg::Vec2(0.0, (bHasLogo ? 0.33 : 0.25)));
                // Second, bottom vertex
                pit_vtx->push_back(osg::Vec3(x3, y3, z3));
                pit_tex->push_back(osg::Vec2(1.0, 0.0));
                // Second, top vertex
                pit_vtx->push_back(osg::Vec3(x3, y3, z3 + 0.9));
                pit_tex->push_back(osg::Vec2(1.0, (bHasLogo ? 0.33 : 0.25)));

                osg::Geometry *geometry = new osg::Geometry;
                geometry->setVertexArray(pit_vtx);
                geometry->setTexCoordArray(0, pit_tex, osg::Array::BIND_PER_VERTEX);
                geometry->setNormalArray(pit_nrm, osg::Array::BIND_OVERALL);
                geometry->setColorArray(pit_clr, osg::Array::BIND_OVERALL);
                geometry->addPrimitiveSet( new osg::DrawArrays(osg::PrimitiveSet::TRIANGLE_STRIP, 0, pit_vtx->size()) );

                osg::Geode *logo = new osg::Geode;
                logo->addDrawable(geometry);

                osg::StateSet *stateSet = logo->getOrCreateStateSet();
                stateSet->setMode(GL_CULL_FACE, osg::StateAttribute::OFF);
                stateSet->setMode(GL_BLEND, osg::StateAttribute::OFF);

                osg::Texture2D *texture = wallTexture;
                if (bHasLogo) {
                    // Load logo texture (.png first, then .rgb for backwards compatibility)
                    osg::ref_ptr<osg::Image> image = loader.LoadImageFile(strLogoFileName + ".png");
                    if (!image) image = loader.LoadImageFile(strLogoFileName + ".rgb");
                    if (image) {
                        texture = new osg::Texture2D;
                        texture->setDataVariance(osg::Object::STATIC);
                        texture->setWrap(osg::Texture2D::WRAP_S, osg::Texture2D::CLAMP_TO_EDGE);
                        texture->setWrap(osg::Texture2D::WRAP_T, osg::Texture2D::CLAMP_TO_EDGE);
                        texture->setMaxAnisotropy(16);
                        texture->setImage(image);
                    }
                }
                if (texture)
                    stateSet->setTextureAttributeAndModes(0, texture, osg::StateAttribute::ON);

                pit_root->addChild(logo);
            }

            // load pit indicator
            if (bHasPitIndicator)
            {
                loader.AddSearchPath(std::string("tracks/") + track->category + "/" + track->internalname);
                loader.AddSearchPath("data/objects");
                loader.AddSearchPath("data/textures");
                loader.AddSearchPath("data/img");

                std::string filename = pits->pitindicator == 1
                                     ? "pit_indicator.ac"
                                     : "normal_pit_indicator.ac";
                osg::Node *node = loader.Load3dFile(filename);

                osg::MatrixTransform *transform = new osg::MatrixTransform;
                transform->setMatrix(osg::Matrix::translate(x3, y3, z3));
                transform->addChild(node);

                pit_root->addChild(transform);
            }
        }  // for i
    }
        break;

    case TR_PIT_ON_SEPARATE_PATH:
        // Not implemented yet
        GfLogTrace("Creating separate path pits (%d slots) ...\n", pits->nMaxPits);
        GfLogWarning("Separate path pits are not yet implemented.\n");
        break;

    case TR_PIT_NONE:
        // Nothing to do
        GfLogTrace("Creating no pits.\n");
        break;
    }  // switch pits->type
}  // SDPits::build

