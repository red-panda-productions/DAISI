/***************************************************************************

    file        : MyTrack.h
    created     : 9 Apr 2006
    copyright   : (C) 2006 Tim Foden, 2013 D.Schellhammer

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#ifndef _MYTRACK_H_
#define _MYTRACK_H_

#include <track.h>
#include <car.h>

#include "Seg.h"


class MyTrack
{
public:
  MyTrack();
  ~MyTrack();

  void NewTrack( tTrack* pNewTrack, double seg_len );
  int  GetSize() const;
  const Seg& operator[]( int index ) const;

private:
  void CalcPtAndNormal( const tTrackSeg* pSeg, double toStart, double& t, Vec3d& pt, Vec3d& norm ) const;

private:
  int  NSEG;
  double m_delta;
  Seg* m_pSegs;
  tTrack* m_pCurTrack;
};

#endif // _MYTRACK_H_
