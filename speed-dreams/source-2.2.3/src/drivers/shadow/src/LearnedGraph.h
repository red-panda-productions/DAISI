/***************************************************************************

    file        : LearnedGraph.h
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

#ifndef _LEARNEDGRAPH_H_
#define _LEARNEDGRAPH_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class LearnedGraph
{
public:
    LearnedGraph();
    LearnedGraph( int nAxes, const double* minX, const double* maxX,
                    const int* xSteps, double initialValue  );
    LearnedGraph( double minX, double maxX, int xSteps, double initialY );
    ~LearnedGraph();

    int		GetNAxes() const;
    int		GetAxisSize( int axis ) const;

    void	Learn( double x, double value );
    void	Learn( const double* coord, double value );

    double	CalcY( double x ) const;
    double	CalcValue( const double* coord ) const;

    double	GetY( int index ) const;
    double	GetValue( const int* index ) const;

    void	SetBeta( double beta );

private:
    struct Axis
    {
        double	m_min;
        double	m_span;
        int		m_steps;
        int		m_itemSize;
    };

    struct Idx
    {
        int		i;
        int		j;
        double	t;
    };

private:
    double	CalcValue( int dim, int offs, const Idx* idx ) const;
    void	LearnValue( int dim, int offs, const Idx* idx, double delta );
    Idx*	MakeIdx( const double* coord ) const;

private:
    int		m_nAxes;
    Axis*	m_pAxis;
//	int		m_steps;
//	double	m_minX;
//	double	m_spanX;
    double	m_beta;
    double*	m_pData;
};

#endif // _LEARNEDGRAPH_H_
