#ifndef _OPTIMISEDPATH_H_
#define _OPTIMISEDPATH_H_

#include "Array.h"
#include "LinePath.h"
#include "CarModel.h"

class OptimisedPath : public LinePath
{
public:
	OptimisedPath();
	virtual ~OptimisedPath();

	virtual LinePath&	operator=( const LinePath& path )
	{
		return LinePath::operator=(path);
	}

	void Initialise( MyTrack* pTrack, const CarModel& cm,
					 double maxL, double maxR );

private:
	// at each point we have multiple velocities possible, each with a time.
	// for each arrival point for a given range of velocities, we choose
	//	the one that has the least time.
	// we cull nodes that have times that are way to slow, and velocities that are
	//	way too far from the mode.
	//
	//	21 * 314 ~ 600.  2 bytes for id, so 1200 bytes per slice.
	//	in 10km track are 10000/3 = 3333 slices, so 1200 * 3333 ~ 4mb

	struct Node
	{
		double	m_offset;	// across track (m).
		double	m_time;		// to get here (s).
		int		m_from;		// where we came from.
		Vec2d	m_vel;		// vector velocity (m/s).
		double	m_spd;		// speed.
		double	m_ang;		// velocity angle.
	};

	class NodeArray : public Array<Node>
	{
	};

	class Slice
	{
	public:
		int			m_seg;			// which seg this slice represents;
		double		m_midOffset;
		Vec2d		m_midVelocity;
		NodeArray	m_nodes;
	};

	class Trace
	{
	public:
		Array<int>	m_from;
	};

private:
	void Initialise();
	void SetupStartSlice();
	void SetupSlice( int seg, Slice* pSlice );
	void NextSlice();

private:
	Array<Trace>	m_traces;
	CarModel		m_cm;
	Slice*			m_curSlice;
	Slice*			m_nextSlice;
	double			m_offsetRadius;
	double			m_offsetStep;
	double			m_velocityRadius;
	Vec2d			m_velocityStep;
};

#endif
