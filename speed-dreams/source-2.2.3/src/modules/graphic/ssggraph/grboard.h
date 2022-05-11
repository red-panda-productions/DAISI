/***************************************************************************

    file                 : grboard.h
    created              : Thu Aug 17 23:55:47 CEST 2000
    copyright            : (C) 2000 by Eric Espie
    email                : torcs@free.fr
    version              : $Id: grboard.h 7003 2020-04-30 03:34:34Z iobyte $

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef _GRBOARD_H_
#define _GRBOARD_H_

#include <plib/ssg.h>

#include <car.h>      // tCarElt
#include <raceman.h>  // tSituation

class cGrFrameInfo;

#include <string>
#include <vector>

#include "IndicatorData.h"

class cGrBoard
{
protected:
    int id;  // Board Id
    const tCarElt *car_;

    int debugFlag;
    int boardWidth;
    int leftAnchor;
    int centerAnchor;
    int rightAnchor;
    int speedoRise;

private:
    void grDispDebug(const tSituation *s, const cGrFrameInfo *frame);
    void grDispSplitScreenIndicator();
    void grDrawGauge(tdble X1, tdble Y1, tdble H, float *clr1,
                     float *clr2, tdble val, const char *title);

    void grDispCounterBoard2();

    // SIMULATED DRIVING ASSISTANCE: add display indicator methods
    void DispActiveIndicators(tIndicator settings, InterventionType interventionType);
    void DispNeutralIndicators(tIndicator settings, InterventionType interventionType);
    void DispIndicatorsHelper(tIndicatorData m_indicator, tIndicator settings);
    void DispIndicatorIcon(tTextureData *p_data, ssgSimpleState *p_texture);
    void DispIndicatorText(tTextData *p_data);

private:
    // Dash colour handling
    float *normal_color_;
    float *danger_color_;
    float *emphasized_color_;
    float *background_color_;

    void ReadDashColor(void *hdle, const std::string &color_name, float **color);

public:
    explicit cGrBoard(int myid);
    ~cGrBoard();

    void initBoard(void);
    void shutdown(void);
    void selectBoard(int brd);
    void setWidth(int width);

    void refreshBoard(tSituation *s, const cGrFrameInfo *frameInfo,
                      const tCarElt *currCar, bool isCurrScreen);
    void loadDefaults(const tCarElt *curCar);
};

extern void grInitBoardCar(tCarElt *car);

// SIMULATED DRIVING ASSISTANCE
extern void LoadIndicatorTextures();

#endif /* _GRBOARD_H_ */
