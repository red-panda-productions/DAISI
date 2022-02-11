/***************************************************************************

    file        : MyTrack.cpp
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


#include "MyTrack.h"
#include "Utils.h"

#include <robottools.h>


MyTrack::MyTrack() : NSEG(0), m_delta(3), m_pSegs(0), m_pCurTrack(0)
{
}

MyTrack::~MyTrack()
{
  delete [] m_pSegs;
}

void MyTrack::NewTrack( tTrack* pNewTrack, double seg_len )
{
  if( m_pCurTrack != pNewTrack )
  {
    delete [] m_pSegs;
    m_pSegs = 0;
    NSEG = 0;
  }

  m_pCurTrack = pNewTrack;

  if( m_pSegs == 0 )
  {
    // make new segs ... roughly every NOMINAL_SEG_LEN metres apart.
    const double NOMINAL_SEG_LEN = seg_len;
    NSEG = int(floor(pNewTrack->length / NOMINAL_SEG_LEN));
    m_pSegs = new Seg[NSEG];
    m_delta = pNewTrack->length / NSEG;

    tTrackSeg* pseg = pNewTrack->seg;
    while( pseg->lgfromstart > pNewTrack->length / 2 )
      pseg = pseg->next;
    double  tsend = pseg->lgfromstart + pseg->length;

    for( int i = 0; i < NSEG; i++ )
    {
      double segDist = i * m_delta;
      while( segDist >= tsend )
      {
        pseg = pseg->next;
        tsend = pseg->lgfromstart + pseg->length;
      }

      m_pSegs[i].segDist = segDist;
      m_pSegs[i].pSeg = pseg;
      m_pSegs[i].wl = pseg->width / 2;
      m_pSegs[i].wr = pseg->width / 2;
      m_pSegs[i].midOffs = 0;
    }

    for( int i = 0; i < NSEG; i++ )
    {
      pseg = m_pSegs[i].pSeg;
      double segDist = m_pSegs[i].segDist;
      CalcPtAndNormal( pseg, segDist - pseg->lgfromstart,
                       m_pSegs[i].t,
                       m_pSegs[i].pt, m_pSegs[i].norm );
    }
  }
}

int  MyTrack::GetSize() const
{
  return NSEG;
}

const Seg& MyTrack::operator[]( int index ) const
{
 return m_pSegs[index];
}

void MyTrack::CalcPtAndNormal(const tTrackSeg* pSeg, double toStart, double& t, Vec3d& pt, Vec3d& norm ) const
{
  if( pSeg->type == TR_STR )
  {
    Vec3d s = (Vec3d(pSeg->vertex[TR_SL]) + Vec3d(pSeg->vertex[TR_SR])) / 2;
    Vec3d e = (Vec3d(pSeg->vertex[TR_EL]) + Vec3d(pSeg->vertex[TR_ER])) / 2;
    t = toStart / pSeg->length;
    pt = s + (e - s) * t;

    double hl = pSeg->vertex[TR_SL].z + (pSeg->vertex[TR_EL].z - pSeg->vertex[TR_SL].z) * t;
    double hr = pSeg->vertex[TR_SR].z + (pSeg->vertex[TR_ER].z - pSeg->vertex[TR_SR].z) * t;
    norm = -Vec3d(pSeg->rgtSideNormal);
    norm.z = (hr - hl) / pSeg->width;
  }
  else
  {
    double d = pSeg->type == TR_LFT ? 1 : -1;
    double deltaAng = d * toStart / pSeg->radius;
    double ang = pSeg->angle[TR_ZS] - PI / 2 + deltaAng;
    double c = cos(ang);
    double s = sin(ang);
    double r = d * pSeg->radius;
    t = toStart / pSeg->length;
    double hl = pSeg->vertex[TR_SL].z + (pSeg->vertex[TR_EL].z - pSeg->vertex[TR_SL].z) * t;
    double hr = pSeg->vertex[TR_SR].z + (pSeg->vertex[TR_ER].z - pSeg->vertex[TR_SR].z) * t;
    pt = Vec3d(pSeg->center.x + c * r, pSeg->center.y + s * r, (hl + hr) / 2);
    norm = Vec3d(c, s, (hr - hl) / pSeg->width);
  }
}
