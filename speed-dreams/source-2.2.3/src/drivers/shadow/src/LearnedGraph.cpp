/***************************************************************************

    file        : LearnedGraph.cpp
    created     : 9 Apr 2006
    copyright   : (C) 2006 Tim Foden

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "LearnedGraph.h"
#include <math.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

LearnedGraph::LearnedGraph()
:	m_nAxes(0),
    m_pAxis(0),
    m_beta(0.5),
    m_pData(0)
{
}

LearnedGraph::LearnedGraph(
    int				nAxes,
    const double*	min,
    const double*	max,
    const int*		steps,
    double			initialValue )
:
    m_nAxes(nAxes),
    m_pAxis(0),
    m_beta(0.5),
    m_pData(0)
{
    m_pAxis = new Axis[nAxes];
    int	itemSize = 1;
    for( int i = nAxes - 1; i >= 0; i-- )
    {
        m_pAxis[i].m_min = min[i];
        m_pAxis[i].m_span = max[i] - min[i];
        m_pAxis[i].m_steps = steps[i];
        m_pAxis[i].m_itemSize = itemSize;

        itemSize *= steps[i] + 1;
    }

    m_pData = new double[itemSize];
    {for( int i = 0; i < itemSize; i++ )
        m_pData[i] = initialValue;
    }
}

LearnedGraph::LearnedGraph( double minX, double maxX, int xSteps, double initialY )
:	m_nAxes(1),
    m_pAxis(0),
    m_beta(0.5),
    m_pData(0)
{
    m_pAxis = new Axis[1];
    m_pAxis->m_min = minX;
    m_pAxis->m_span = maxX - minX;
    m_pAxis->m_steps = xSteps;
    m_pAxis->m_itemSize = 1;

    m_pData = new double[xSteps + 1];
    for( int i = 0; i <= xSteps; i++ )
        m_pData[i] = initialY;
}

LearnedGraph::~LearnedGraph()
{
    delete [] m_pData;
}

int		LearnedGraph::GetNAxes() const
{
    return m_nAxes;
}

int		LearnedGraph::GetAxisSize( int axis ) const
{
    if( axis < 0 || axis >= m_nAxes )
        return 0;

    return m_pAxis[axis].m_steps + 1;
}

void	LearnedGraph::Learn( double x, double value )
{
    // ASSERT( m_nAxes == 1 );
/*	Idx*	idx = MakeIdx(&x);

    double	oldValue = m_pData[idx->i] * (1 - idx->t) +
                       m_pData[idx->j] * idx->t;

    double	delta = m_beta * (value - oldValue);

    m_pData[idx->i] += delta * (1 - idx->t);
    m_pData[idx->j] += delta * idx->t;

    delete [] idx;*/
    Learn( &x, value );
}

void	LearnedGraph::Learn( const double* coord, double value )
{
    Idx*	idx = MakeIdx(coord);

    double	oldValue = CalcValue(0, 0, idx);
    double	delta = m_beta * (value - oldValue);
    LearnValue( 0, 0, idx, delta );

    delete [] idx;
}

double	LearnedGraph::CalcY( double x ) const
{
/*	double	t = m_steps * (x - m_minX) / m_spanX;	// 0 <= t <= m_steps
    if( t < 0 )
        t = 0;
    else if( t > m_steps )
        t = m_steps;

    int	idx0 = (int)floor(t);
    int	idx1 = idx0 < m_steps ? idx0 + 1 : m_steps;

    t = t - idx0;	// 0 <= t <= 1
    return m_pData[idx0] * (1 - t) + m_pData[idx1] * t;*/
    return CalcValue(&x);
}

double	LearnedGraph::CalcValue( const double* coord ) const
{
    Idx*	idx = MakeIdx(coord);
    double	value = CalcValue(0, 0, idx);
    delete [] idx;
    return value;
}

double	LearnedGraph::GetY( int index ) const
{
    return m_pData[index];
}

double	LearnedGraph::GetValue( const int* index ) const
{
    int	idx = 0;
    for( int i = 0; i < m_nAxes; i++ )
        idx += m_pAxis[i].m_itemSize * index[i];
    return m_pData[idx];
}

void	LearnedGraph::SetBeta( double beta )
{
    m_beta = beta;
}

double	LearnedGraph::CalcValue( int dim, int offs, const Idx* idx ) const
{
    if( dim < m_nAxes )
    {
        int		offs_i = offs + m_pAxis[dim].m_itemSize * idx[dim].i;
        int		offs_j = offs + m_pAxis[dim].m_itemSize * idx[dim].j;

        double	a = CalcValue(dim + 1, offs_i, idx);
        double	b = CalcValue(dim + 1, offs_j, idx);

        return a * (1 - idx[dim].t) + b * idx[dim].t;
    }
    else
        return m_pData[offs];
}

void	LearnedGraph::LearnValue( int dim, int offs, const Idx* idx, double delta )
{
    if( dim < m_nAxes )
    {
        int		offs_i = offs + m_pAxis[dim].m_itemSize * idx[dim].i;
        int		offs_j = offs + m_pAxis[dim].m_itemSize * idx[dim].j;

        LearnValue( dim + 1, offs_i, idx, delta * (1 - idx[dim].t) );
        LearnValue( dim + 1, offs_j, idx, delta * idx[dim].t );
    }
    else
        m_pData[offs] += delta;
}

LearnedGraph::Idx*	LearnedGraph::MakeIdx( const double* coord ) const
{
    Idx*	idx = new Idx[m_nAxes];

    for( int i = 0; i < m_nAxes; i++ )
    {
        // 0 <= t <= m_steps
        idx[i].t = m_pAxis[i].m_steps * (coord[i] - m_pAxis[i].m_min) /
                            m_pAxis[i].m_span;
        if( idx[i].t < 0 )
            idx[i].t = 0;
        else if( idx[i].t > m_pAxis[i].m_steps )
            idx[i].t = m_pAxis[i].m_steps;

        idx[i].i = (int)floor(idx[i].t);
        idx[i].j = idx[i].i < m_pAxis[i].m_steps ? idx[i].i + 1 : m_pAxis[i].m_steps;
        idx[i].t = idx[i].t - idx[i].i;	// 0 <= t <= 1
    }

    return idx;
}
