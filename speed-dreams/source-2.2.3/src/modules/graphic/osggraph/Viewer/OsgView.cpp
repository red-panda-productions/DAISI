/***************************************************************************

    file        : OsgViewer.cpp
    created     : Sun Jan 13 22:11:03 CEST 2013
    copyright   : (C) 2013 by Xavier Bertaux
    email       : bertauxx@yahoo.fr
    version     : $Id$

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

// include RenderInfo first which contains a word "_style"
// which can be replaced by the same preprocessor definition from <car.h>
// and GCC will raise a strange untrackable compilation error
#include <osg/RenderInfo>
#include <osg/Camera>

#include <tgfclient.h>
#include <graphic.h>

#include "OsgMain.h"
#include "OsgView.h"
#include "OsgCar.h"
#include "OsgNodeMask.h"

static char buf[1024];
static char path[1024];
static char path2[1024];

SDView::SDView(int id, osg::Camera * c, int x, int y, int width, int height,
                osg::Camera * mc)
{
    this->x =x;
    this->y =y;
    this->width= width;
    this->height = height;
    viewOffset =0;
    cam = c;
    camFrontFace = new osg::FrontFace();
    cam->getOrCreateStateSet()->setAttribute(camFrontFace);

    mirrorCam = mc;
    hasChangedMirrorFlag = false;
    mirrorFlag = true;
    mirrorCamFrontFace = new osg::FrontFace();
    mirrorCam->getOrCreateStateSet()->setAttribute(mirrorCamFrontFace);


    tdble fovFactor = 1;
    tdble fixedFar = 80000.0;
    mirror = new SDCarCamMirror(
                this,
                -1,
                0,					// drawCurr
                1,					// drawBG
                50.0,				// fovy
                0.0,				// fovymin
                360.0,				// fovymax
                0.3,				// near
                fixedFar ? fixedFar : 300.0 * fovFactor, // far
                fixedFar ? 2*fixedFar/3 : 200.0 * fovFactor,	// fogstart
                fixedFar ? fixedFar : 300.0 * fovFactor	// fogend
                           );
    // mirror->setProjection();

    this->id = id;
    curCar = NULL;
    cars = 0;
    selectNextFlag=false;
    selectPrevFlag=false;
    mirrorFlag=false;
}

SDView::~SDView()
{
    delete cameras;
}

void SDView::switchMirror(void)
{
    mirrorFlag = 1 - mirrorFlag;
    hasChangedMirrorFlag = true;
    sprintf(path, "%s/%d", GR_SCT_DISPMODE, id);
    GfParmSetNum(grHandle, path, GR_ATT_MIRROR, NULL, (tdble)mirrorFlag);

    if (curCar->_driverType == RM_DRV_HUMAN)
    {
        sprintf(path2, "%s/%s", GR_SCT_DISPMODE, curCar->_name);
        GfParmSetNum(grHandle, path2, GR_ATT_MIRROR, NULL, (tdble)mirrorFlag);
    }

    GfParmWriteFile(NULL, grHandle, "Graph");
}

void SDView::Init(tSituation *s)
{
    cameras = new SDCameras(this, s->_ncars);
    loadParams(s);
}

void SDView::setCurrentCar(tCarElt *newCurCar)
{
    curCar = newCurCar;
    ((SDCars*)getCars())->markCarCurrent(curCar);
}

/* Update screen display */
void SDView::update(tSituation *s, const SDFrameInfo* frameInfo)
{
    int carChanged = 0;
    if (selectNextFlag)
    {
        for (int i = 0; i < (s->_ncars - 1); i++)
        {
            if (curCar == s->cars[i])
            {
                curCar = s->cars[i + 1];
                carChanged = 1;
                GfOut("Car Next\n");
                break;
            }
        }

        selectNextFlag = false;
    }

    if (selectPrevFlag)
    {
        for (int i = 1; i < s->_ncars; i++)
        {
            if (curCar == s->cars[i])
            {
                curCar = s->cars[i - 1];
                carChanged = 1;
                GfOut("Car Previous\n");
                break;
            }
        }

        selectPrevFlag = false;
    }

    if (carChanged)
    {
        sprintf(path, "%s/%d", GR_SCT_DISPMODE, id);
        GfParmSetStr(grHandle, path, GR_ATT_CUR_DRV, curCar->_name);
        loadParams (s);
        //board->setWidth(fakeWidth);
        GfParmWriteFile(NULL, grHandle, "Graph");
        ((SDCars*)getCars())->markCarCurrent(curCar);
    }

    if(hasChangedMirrorFlag)
    {
        hasChangedMirrorFlag =false;
        this->de_activateMirror();
    }

    cameras->update(curCar, s);
    mirror->update(curCar, s);
    mirror->setModelView();
    cam->setCullMask( cameras->getSelectedCamera()->getCullMask() );
    mirrorCam->setCullMask( mirror->getCullMask() );
}

void SDView::activate(int x, int y, int width, int height, float v)
{
    this->x =x;
    this->y =y;
    this->width= width;
    this->height = height;
    cameras->getSelectedCamera()->setViewOffset(v);
    viewOffset =v;
    cam->setViewport(new osg::Viewport(x,y,width,height));
    cameras->getSelectedCamera()->setProjection();
    cam->setNodeMask(NODE_MASK_ALL);
    this->de_activateMirror();
}

void SDView::deactivate()
{
    cam->setNodeMask(NODE_MASK_NONE);
    mirrorCam->setNodeMask(NODE_MASK_NONE);
}

void SDView::de_activateMirror()
{
    mirror->adaptScreenSize();
    if(mirrorFlag)
    {
        if(cameras->getSelectedCamera()->getMirrorAllowed())
        {
            this->mirrorCam->setNodeMask(NODE_MASK_ALL);
        }else
        {
            this->mirrorCam->setNodeMask(NODE_MASK_NONE);
        }
    }else
    {
        this->mirrorCam->setNodeMask(NODE_MASK_NONE);
    }
}

Camera* SDView::getCamera()
{
    return cameras->getSelectedCamera()->getGenericCamera();
}

void SDView::loadParams(tSituation *s)
{
    int camNum;
    int camList;
    int i;

    const char *carName;
    const char *pszSpanSplit;

    // Initialize the screen "current car" if not already done.
    sprintf(path, "%s/%d", GR_SCT_DISPMODE, id);

    if (!curCar)
    {
        // Load the name of the "current driver", and search it in the race competitors.
        carName = GfParmGetStr(grHandle, path, GR_ATT_CUR_DRV, "");
        for (i = 0; i < s->_ncars; i++)
        {
            if (!strcmp(s->cars[i]->_name, carName))
            {
                break;
            }
        }

        // Found : this is the "current driver".
        if (i < s->_ncars)
        {
            curCar = s->cars[i];
        } else if (id < s->_ncars)
        {
            curCar = s->cars[id];
        } else
        {
            curCar = s->cars[0];
        }

        GfLogTrace("Screen #%d : Assigned to %s\n", id, curCar->_name);
    }

    // Load "current camera" settings (attached to the "current car").
    camList	= (int)GfParmGetNum(grHandle, path, GR_ATT_CAM_HEAD, NULL, 9);
    camNum	= (int)GfParmGetNum(grHandle, path, GR_ATT_CAM, NULL, 0);
    mirrorFlag = (int)GfParmGetNum(grHandle, path, GR_ATT_MIRROR, NULL, (tdble)mirrorFlag);

    // Only apply driver preferences when not spanning split screens
    pszSpanSplit = GfParmGetStr(grHandle, GR_SCT_GRAPHIC, GR_ATT_SPANSPLIT, GR_VAL_NO);

    if (strcmp(pszSpanSplit, GR_VAL_YES) && curCar->_driverType == RM_DRV_HUMAN)
    {
        sprintf(path2, "%s/%s", GR_SCT_DISPMODE, curCar->_name);
        camList	= (int)GfParmGetNum(grHandle, path2, GR_ATT_CAM_HEAD, NULL, (tdble)camNum);
        camNum	= (int)GfParmGetNum(grHandle, path2, GR_ATT_CAM, NULL, (tdble)camList);
        mirrorFlag	= (int)GfParmGetNum(grHandle, path2, GR_ATT_MIRROR, NULL, (tdble)mirrorFlag);
    }

    // Retrieve the "current camera".
    cameras->selectCamera(camList, camNum);

    // Back to the default camera if not found (and save it as the new current one).
    cameras->getIntSelectedListAndCamera(&camList,&camNum);
    GfParmSetNum(grHandle, path, GR_ATT_CAM, NULL, (tdble)camNum);
    GfParmSetNum(grHandle, path, GR_ATT_CAM_HEAD, NULL, (tdble)camList);

    sprintf(buf, "%s-%d-%d", GR_ATT_FOVY, camList, camNum);
    cameras->getSelectedCamera()->loadDefaults(buf);
}

void SDView::saveCamera()
{
    int camList, camNum;

    cameras->getIntSelectedListAndCamera(&camList, &camNum);

    sprintf(path, "%s/%d", GR_SCT_DISPMODE, id);
    GfParmSetStr(grHandle, path, GR_ATT_CUR_DRV, curCar->_name);
    GfParmSetNum(grHandle, path, GR_ATT_CAM, (char*)NULL, (tdble)camNum);
    GfParmSetNum(grHandle, path, GR_ATT_CAM_HEAD, (char*)NULL, (tdble)camList);

    /* save also as user's preference if human */
    if (curCar->_driverType == RM_DRV_HUMAN)
    {
        sprintf(path2, "%s/%s", GR_SCT_DISPMODE, curCar->_name);
        GfParmSetNum(grHandle, path2, GR_ATT_CAM, (char*)NULL, (tdble)camNum);
        GfParmSetNum(grHandle, path2, GR_ATT_CAM_HEAD, (char*)NULL, (tdble)camList);
    }

    sprintf(buf, "%s-%d-%d", GR_ATT_FOVY, camList, camNum);
    GfParmWriteFile(NULL, grHandle, "Graph");
    GfLogDebug("Written screen=%d camList=%d camNum=%d\n",id,camList,camNum);
}
