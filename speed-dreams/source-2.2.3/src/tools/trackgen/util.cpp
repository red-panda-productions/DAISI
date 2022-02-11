/***************************************************************************

    file                 : util.cpp
    created              : Wed May 29 22:20:24 CEST 2002
    copyright            : (C) 2001 by Eric Espie
    email                : eric.espie@torcs.org
    version              : $Id: util.cpp 6759 2019-11-09 12:08:50Z torcs-ng $

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

/** @file

    @author	<a href=mailto:eric.espie@torcs.org>Eric Espie</a>
    @version	$Id: util.cpp 6759 2019-11-09 12:08:50Z torcs-ng $
*/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <cstring>
#ifndef WIN32
#include <unistd.h>
#endif
#include <math.h>

#include <plib/ul.h>
#include <tgfclient.h>
#include <track.h>
#include <robottools.h>
#include "trackgen.h"
#include "util.h"

tdble Distance(tdble x0, tdble y0, tdble z0, tdble x1, tdble y1, tdble z1)
{
    return sqrt((x0-x1)*(x0-x1)+(y0-y1)*(y0-y1)+(z0-z1)*(z0-z1));
}

int
GetFilename(const char *filename, const char *filepath, char *buf)
{
    const char	*c1, *c2;
    int		found = 0;
    int		lg;

    if (filepath)
    {
        c1 = filepath;
        c2 = c1;

        while ((!found) && (c2 != NULL))
        {
            c2 = strchr(c1, ';');

            if (c2 == NULL)
            {
                sprintf(buf, "%s/%s", c1, filename);
            }
            else
            {
                lg = c2 - c1;
                strncpy(buf, c1, lg);
                buf[lg] = '/';
                strcpy(buf + lg + 1, filename);
            }

            if (ulFileExists(buf))
            {
                found = 1;
            }
            c1 = c2 + 1;
        }
    }
    else
    {
        strcpy(buf, filename);
        if (ulFileExists(buf))
        {
            found = 1;
        }
    }

    if (!found)
    {
        printf("File %s not found\n", filename);
        printf("File Path was %s\n", filepath);

        return 0;
    }

    return 1;
}

float
getHOT(ssgRoot *root, float x, float y)
{
    sgVec3 test_vec;
    sgMat4 invmat;
    sgMakeIdentMat4(invmat);

    invmat[3][0] = -x;
    invmat[3][1] = -y;
    invmat[3][2] =  0.0f         ;

    test_vec [0] = 0.0f;
    test_vec [1] = 0.0f;
    test_vec [2] = 100000.0f;

    ssgHit *results;
    int num_hits = ssgHOT (root, test_vec, invmat, &results);

    float hot = -1000000.0f;

    for (int i = 0; i < num_hits; i++)
    {
        ssgHit *h = &results[i];

        float hgt = - h->plane[3] / h->plane[2];

        if (hgt >= hot)
            hot = hgt;
    }

    return hot;
}
/*
 * calculates an angle that aligns with the closest track segment. Angle
* is determined so that the x axis is aligned parallel to track,
*  with positive y axis pointing away from track
*  Finished
*/
float
getTrackAngle(tTrack *Track, void *TrackHandle, float x, float y)
{
    float angle=0, dst=100000, shDst=100000;
    int 		i;
    tTrackSeg 		*seg = Track->seg;
    tTrkLocPos		curPos, closePos, objPos;
    float x2, y2, x3, y3;
    for(i = 0; i < Track->nseg; i++)
    {
        seg=seg->next;
        curPos.seg=seg;
        curPos.toMiddle=0;

        switch (seg->type)
        {
        case TR_STR:
            for (float toStart=0; toStart<seg->length; toStart+=(seg->length/5.0))
            {
                curPos.toStart=toStart;
                RtTrackLocal2Global(&curPos, &x2, &y2, TR_TOMIDDLE);
                dst=Distance(x,y,0,x2,y2,0);
                //printf("distance: %g\n",dst);

                if ( dst < shDst )
                {
                    shDst=dst;
                    closePos=curPos;
                }
            }

            break;
        case TR_RGT:
            for (float toStart=0; toStart<seg->arc; toStart+=(seg->arc/5.0))
            {
                curPos.toStart=toStart;
                RtTrackLocal2Global(&curPos, &x2, &y2, TR_TOMIDDLE);
                dst=Distance(x,y,0,x2,y2,0);
                //printf("distance: %g\n",dst);

                if ( dst < shDst )
                {
                    shDst=dst;
                    closePos=curPos;
                }
            }

            break;
        case TR_LFT:
            for (float toStart=0; toStart<seg->arc; toStart+=(seg->arc/10.0))
            {
                curPos.toStart=toStart;
                RtTrackLocal2Global(&curPos, &x2, &y2, TR_TOMIDDLE);
                dst=Distance(x,y,0,x2,y2,0);
                //printf("distance: %g\n",dst);

                if ( dst < shDst )
                {
                    shDst=dst;
                    closePos=curPos;
                }
            }

            break;
        }
    }

    printf("shortest distance: %g\n",shDst);
    curPos=closePos;
    seg=curPos.seg;
    printf("found at segment: %s\n",seg->name);
    RtTrackLocal2Global(&curPos, &x3, &y3, TR_TOMIDDLE);
    curPos.toStart+=0.001;
    RtTrackLocal2Global(&curPos, &x2, &y2, TR_TOMIDDLE);
    angle=90.0-atan2f(x2-x3,y2-y3)*180/PI;
    printf("calculated angle%g\n",angle);
    RtTrackGlobal2Local(seg, x, y, &objPos, 0);
    if ( objPos.toMiddle < 0 ) {angle-=180.0;}
    return angle;
}

/*
 * same thing for border alignment, just one difference: a new position
 * x/y/z is returned to keep it aligned with the track in distance & height
 * need additional input in xml object definition:
 *   <attnum...
 *
 */
float
getBorderAngle(tTrack *Track, void *TrackHandle, float x, float y, float distance, float *xRet, float *yRet, float *zRet )
{
    float angle=0, dst=100000, shDst=100000;
    int 		i;
    tTrackSeg 		*seg = Track->seg;
    tTrkLocPos		curPos, closePos, objPos;
    float x2, y2, x3, y3;
    for(i = 0; i < Track->nseg; i++)
    {
        seg=seg->next;
        curPos.seg=seg;
        curPos.toMiddle=0;

        switch (seg->type)
        {
        case TR_STR:
            for (float toStart=0; toStart<seg->length; toStart+=(seg->length/5.0))
            {
                curPos.toStart=toStart;
                RtTrackLocal2Global(&curPos, &x2, &y2, TR_TOMIDDLE);
                dst=Distance(x,y,0,x2,y2,0);
                //printf("distance: %g\n",dst);
                if ( dst < shDst ){
                    shDst=dst;
                    closePos=curPos;
                }
            }

            break;
        case TR_RGT:
            for (float toStart=0; toStart<seg->arc; toStart+=(seg->arc/5.0))
            {
                curPos.toStart=toStart;
                RtTrackLocal2Global(&curPos, &x2, &y2, TR_TOMIDDLE);
                dst=Distance(x,y,0,x2,y2,0);
                //printf("distance: %g\n",dst);

                if ( dst < shDst )
                {
                    shDst=dst;
                    closePos=curPos;
                }
            }
            break;

        case TR_LFT:
            for (float toStart=0; toStart<seg->arc; toStart+=(seg->arc/10.0))
            {
                curPos.toStart=toStart;
                RtTrackLocal2Global(&curPos, &x2, &y2, TR_TOMIDDLE);
                dst=Distance(x,y,0,x2,y2,0);
                //printf("distance: %g\n",dst);
                if ( dst < shDst )
                {
                    shDst=dst;
                    closePos=curPos;
                }
            }

            break;
        }
    }
    //check if left or right

    curPos=closePos;
    seg=curPos.seg;
    //check if left or right
    RtTrackGlobal2Local(seg, x, y, &objPos, 1);

    if ( objPos.toMiddle < 0 )
        // object on right side
    {
        curPos.toRight = 0;
        objPos.toRight = -distance;
        RtTrackLocal2Global(&curPos, &x3, &y3, TR_TORIGHT);
        curPos.toStart+=0.001;
        RtTrackLocal2Global(&curPos, &x2, &y2, TR_TORIGHT);
        angle=-90.0-atan2f(x2-x3,y2-y3)*180/PI;
        RtTrackLocal2Global(&objPos, &*xRet, &*yRet, TR_TORIGHT);
    }
    else
        // left side
    {	curPos.toLeft = 0;
        objPos.toLeft = -distance;
        RtTrackLocal2Global(&curPos, &x3, &y3, TR_TOLEFT);
        curPos.toStart+=0.001;
        RtTrackLocal2Global(&curPos, &x2, &y2, TR_TOLEFT);
        angle=90.0-atan2f(x2-x3,y2-y3)*180/PI;
        RtTrackLocal2Global(&objPos, &*xRet, &*yRet, TR_TOLEFT);
    }

    *zRet=RtTrackHeightG(seg, *xRet, *yRet);
    printf("tried to align to border: x: %g y: %g z: %g angle: %g \n", *xRet, *yRet, *zRet, angle);
    //return values

    return angle;
}

/*
 * calculates an angle based on plane equation (face normal) of the
* terrain in this spot. * Angle is determined so that the x axis is
* aligned to a horizontal intersection (i.e. height line) of the
* terrain, with y axis pointing towards uphill
*  FINISHED
*/
float
getTerrainAngle(ssgRoot *root, float x, float y)
{
    sgVec3 test_vec;
    sgMat4 invmat;
    sgMakeIdentMat4(invmat);

    invmat[3][0] = -x;
    invmat[3][1] = -y;
    invmat[3][2] =  0.0f         ;

    test_vec [0] = 0.0f;
    test_vec [1] = 0.0f;
    test_vec [2] = 100000.0f;

    ssgHit *results;
    int num_hits = ssgHOT (root, test_vec, invmat, &results);

    float angle = 0.0;

    for (int i = 0; i < num_hits; i++)
    {
        ssgHit *h = &results[i];

        angle = 180.0 - atan2f( h->plane[0],  h->plane[1]) * 180.0 /PI;

    }

    return angle;
}
