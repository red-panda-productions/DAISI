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

#include <car.h>        // tCarElt
#include <raceman.h>    // tSituation

class cGrFrameInfo;

#include <string>
#include <vector>

class cGrBoard
{
 protected:
    int id;     // Board Id
    const tCarElt* car_;

    int debugFlag;
    int counterFlag;
    int boardWidth;
    int leftAnchor;
    int centerAnchor;
    int rightAnchor;
    int speedoRise;

 private:
    void grDispDebug(const tSituation *s, const cGrFrameInfo* frame);
    void grDispSplitScreenIndicator();
    void grDrawGauge(tdble X1, tdble Y1, tdble H, float *clr1,
                        float *clr2, tdble val, const char *title);

    void grDispCounterBoard2();

    void DispIntervention();

private:
    //Dash colour handling
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

    void refreshBoard(tSituation *s, const cGrFrameInfo* frameInfo,
                        const tCarElt *currCar, bool isCurrScreen);
    void loadDefaults(const tCarElt *curCar);
};

extern void grInitBoardCar(tCarElt *car);

#endif /* _GRBOARD_H_ */
