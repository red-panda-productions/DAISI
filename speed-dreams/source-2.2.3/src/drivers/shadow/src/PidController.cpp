/***************************************************************************

    file        : PidController.cpp
    created     : 18 Apr 2017
    copyright   : (C) 2017 Tim Foden

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

// PidController.cpp: implementation of the PidController class.
//
//////////////////////////////////////////////////////////////////////

#include "PidController.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

PidController::PidController()
:	m_lastPropValue(0),
	m_total(0),
	m_maxTotal(100),
	m_totalRate(0),
	m_p(1),
	m_i(0),
	m_d(0)
{
}

PidController::~PidController()
{
}

double	PidController::Sample( double propValue )
{
	return Sample(propValue, propValue - m_lastPropValue);
}

double	PidController::Sample( double propValue, double diffValue )
{
	m_lastPropValue = propValue;

	double	cntrl = propValue * m_p;

	if( m_d != 0 )
	{
		cntrl += diffValue * m_d;
	}

	if( m_i != 0 )
	{
//		m_total = m_total * (1 - m_totalRate) + value * m_totalRate;
		if( m_totalRate == 0 )
			m_total += propValue;
		else
			m_total += (propValue - m_total) * m_totalRate;
		if( m_total > m_maxTotal )
			m_total = m_maxTotal;
		else if( m_total < -m_maxTotal )
			m_total = -m_maxTotal;
		cntrl += m_total * m_i;
	}

	return cntrl;
}
