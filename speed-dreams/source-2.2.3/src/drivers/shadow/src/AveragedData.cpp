/***************************************************************************

    created              : Sat Mar 18 23:16:38 CET 2006
    copyright            : (C) 2006 by Tim Foden, 2015 by Xavier Bertaux
    email                : bertauxx@yahoo.fr
    version              : $Id: AverageData.cpp 6004 2015-05-09 21:03:25Z torcs-ng $

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <math.h>
#include <robottools.h>
#include "AveragedData.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


AveragedData::AveragedData(
	int nBinsX, double minX, double maxX,
	int nBinsY, double minY, double maxY )
:
	m_xSize(nBinsX),
	m_xMin(minX),
	m_xSpan(maxX - minX),
	m_ySize(nBinsY),
	m_yMin(minY),
	m_ySpan(maxY - minY),
	m_pData(0)
{
	m_pData = new Avg[nBinsX * nBinsY];
}

AveragedData::~AveragedData()
{
	delete [] m_pData;
}

int		AveragedData::GetXSize() const
{
	return m_xSize;
}

int		AveragedData::GetYSize() const
{
	return m_ySize;
}

double	AveragedData::GetAxisValue( int axis, int index ) const
{
	if( axis == 0 )
		return m_xMin + index * m_xSpan / m_xSize;
	else
		return m_yMin + index * m_ySpan / m_ySize;
}

double	AveragedData::GetValueAt( int x, int y ) const
{
	if( m_pData[x + y * m_xSize].count == 0 )
		return 0;

	return m_pData[x + y * m_xSize].value /
				m_pData[x + y * m_xSize].count;
}

void	AveragedData::AddValue( double x, double y, double value )
{
	if( x < m_xMin || x > m_xMin + m_xSpan ||
		y < m_yMin || y > m_yMin + m_ySpan )
		return;

	int	xBin = int(floor((x - m_xMin) * m_xSize / m_xSpan));
	int	yBin = int(floor((y - m_yMin) * m_ySize / m_ySpan));

//	GfOut( "Add Value %d %d\n", xBin, yBin );
	m_pData[xBin + yBin * m_xSize].value += value;
	m_pData[xBin + yBin * m_xSize].count++;
}
