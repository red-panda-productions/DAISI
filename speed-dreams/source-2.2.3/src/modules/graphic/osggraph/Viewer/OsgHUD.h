/***************************************************************************

    file        : OsgHUD.h
    created     : Sun Nov 23 20:12:19 CEST 2014
    copyright   : (C) 2014 by Xavier Bertaux
    email       : Xavier Bertaux
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

#ifndef _OSGHUD_H_
#define _OSGHUD_H_

#include <car.h>        // tCarElt
#include <raceman.h>    // tSituation

#include <osg/Camera>
//#include <osgText/Text>

class SDFrameInfo;

#ifdef HUDDEBUG
class OSGPLOT
{
    private:

    public:
    OSGPLOT(
        float positionX,
        float positionY,
        float width,
        float height,
        float maxValue,
        float minValue,
        float timeFrame,
        float referenceLineAtValue,
        const std::string &Xdata,
        const std::string &Ydata,
        const std::string &title
    );
    ~OSGPLOT();
    float positionX;
    float positionY;
    float width;
    float height;
    float maxValue;
    float minValue;
    float timeFrame;
    float referenceLineAtValue;
    std::string Xdata;
    std::string Ydata;
    std::string title;

    osg::Vec3Array* dataPoints;

    osg::Geometry* osgMainPlotLineGeometry;
    osg::Vec3Array* osgMainPlotLineVertices;

    osg::Geometry* osgReferencePlotLineGeometry;
    osg::Vec3Array* osgReferencePlotLineVertices;

    //osgText::Text* osgTitle;

    osg::ref_ptr<osg::Group> osgGroup;

    osg::ref_ptr <osg::Group> getGroup();

    void appendDataPoint(float x, float y, float z);
    void removeOldDataPoint();
    void recalculateDrawnPoint();
    void drawBackground();
    void update(tSituation *s, const SDFrameInfo* frameInfo,const tCarElt *currCar);

};
#endif

class SDHUD
{
    private:
        osg::ref_ptr<osg::Camera> _cameraHUD;
        tCarElt *_car;

        bool    _debugFlag;
        bool    _arcadeFlag;
        bool    _boardFlag;
        bool    _leaderFlag;
        bool    _counterFlag;
        bool    _GFlag;

        //Dash colour handling
        float *_normal_color;
        float *_danger_color;
        float *_ok_color;
        float *_error_color;
        float *_inactive_color_;
        float *_emphasized_color;
        float *_ahead_color;
        float *_behind_color;
        float *_arcade_color;
        float *_background_color;

        //car data that need to be remembered between frames for the hud
        struct CarData {
            int carLaps;
            float remainingFuelForLaps;
            float laptimeFreezeCountdown;//keep display for x seconds
            float laptimeFreezeTime;
            float timeDiffFreezeCountdown;//keep display for x seconds
            float timeDiffFreezeTime;
            int oldSector;
            int oldLapNumber;

            CarData() : carLaps(0), remainingFuelForLaps(0), laptimeFreezeCountdown(3.0f),
            laptimeFreezeTime(0.0f), timeDiffFreezeCountdown(8.0f), timeDiffFreezeTime(0.0f),
            oldSector(0), oldLapNumber(0)
            {
            }
        };
        const tCarElt *lastCar;
        std::map<const tCarElt *, CarData> carData;

        float hudScale;

        std::map<std::string,osg::Geometry* > hudImgElements;
        std::map<std::string,osg::ref_ptr <osg::Group> > hudGraphElements;

#ifdef HUDDEBUG
        std::map<std::string,OSGPLOT* > plotElements;
#endif

    public:
        SDHUD();
        ~SDHUD();

        void ToggleHUD1();
        void ToggleHUD2();
        void ToggleHUD3();
        void ToggleHUD4();
        void ToogleFPS();
        void ToogleHudBoard();

        osg::Geode* HUDGeode;
        osg::Projection* HUDProjectionMatrix;
        osg::ref_ptr<osg::Camera> camera;

        void CreateHUD( int scrH, int scrW);
        void DispDebug(const tSituation *s, const SDFrameInfo* frame);
        void Refresh(tSituation *s, const SDFrameInfo* frameInfo, const tCarElt *currCar);

        osg::ref_ptr <osg::Group> generateHudFromXmlFile( int scrH, int scrW);

        inline osg::ref_ptr<osg::Camera> getRootCamera()
        {
            return _cameraHUD;
        }
};

#endif //_OSGHUD_H_
