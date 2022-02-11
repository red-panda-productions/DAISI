// OptimisedPath.cpp: implementation of the OptimisedPath class.
//
//////////////////////////////////////////////////////////////////////

#include "OptimisedPath.h"
#include "Utils.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

OptimisedPath::OptimisedPath()
{
}

OptimisedPath::~OptimisedPath()
{
}

void OptimisedPath::Initialise( MyTrack* pTrack, const CarModel& cm, double maxL, double maxR )
{
	LinePath::Initialise(pTrack, maxL, maxR);

	m_cm = cm;

	CalcMaxSpeeds( cm );
	PropagateBreaking( cm );
	PropagateAcceleration( cm );

	Initialise();
	SetupStartSlice();

	for( int i = 1; i < pTrack->GetSize(); i++ )
		NextSlice();
}

void OptimisedPath::Initialise()
{
	m_offsetRadius = 05;
	m_offsetStep = 0.25;
	m_velocityRadius = 0.6;
	m_velocityStep = Vec2d(0.2, 0.2);

	m_traces.SetSize( m_pTrack->GetSize() );
}

void OptimisedPath::SetupStartSlice()
{
	// need to add nodes to this slice, that are around the start pt info.
    if( m_curSlice == 0  )
		m_curSlice = new Slice();

	SetupSlice( 0, m_curSlice );

	for( int i = 0; i < m_curSlice->m_nodes.GetSize(); i++ )
	{
		m_curSlice->m_nodes[i].m_from = 0;
	}
}

void OptimisedPath::SetupSlice( int seg, Slice* pSlice )
{
	const int NSEG = m_pTrack->GetSize();

	int	p = (seg + NSEG - 1) % NSEG;
	int	n = (seg + 1) % NSEG;

	Vec2d	tangent;
	Utils::CalcTangent( GetAt(p).CalcPt().GetXY(),
						GetAt(seg).CalcPt().GetXY(),
						GetAt(n).CalcPt().GetXY(), tangent );
	Vec2d	v = tangent * GetAt(seg).accSpd;

	pSlice->m_seg = seg;
	pSlice->m_midOffset = GetAt(seg).offs;
	pSlice->m_midVelocity = v;
	pSlice->m_nodes.RemoveAll();

	int		nOffs = int(floor(m_offsetRadius / m_offsetStep + 0.5));
	for( int o = -nOffs; o <= nOffs; o++ )
	{
		double offs = pSlice->m_midOffset + m_offsetStep * o;

		Vec2d	dir = Utils::VecUnit(pSlice->m_midVelocity);
		double	spd = pSlice->m_midVelocity.len();

		int		minVX = int(floor(-m_velocityRadius / m_velocityStep.x + 0.5));
		int		maxVX = int(floor( m_velocityRadius / m_velocityStep.x + 0.5));
		int		minVY = int(floor(-m_velocityRadius / m_velocityStep.y + 0.5));
		int		maxVY = int(floor( m_velocityRadius / m_velocityStep.y + 0.5));

		double	vr2 = m_velocityRadius * m_velocityRadius;

		for( int vx = minVX; vx <= maxVX; vx++ )
		{
			if( spd + m_velocityStep.x * vx < 1 )
				continue;

			for( int vy = minVY; vy <= maxVY; vy++ )
			{
				Vec2d	vOffs(m_velocityStep.x * vx, m_velocityStep.y * vy);
				if( vOffs.x * vOffs.x + vOffs.y * vOffs.y > vr2 )
					continue;

				Node	node;
				node.m_time = 0;
				node.m_from = -1;
				node.m_offset = offs;
				node.m_vel = pSlice->m_midVelocity + vOffs * dir;
				node.m_spd = node.m_vel.len();
				node.m_ang = Utils::VecAngle(node.m_vel);

				pSlice->m_nodes.Add( node );
			}
		}
	}
}

void OptimisedPath::NextSlice()
{
	// this is the biggy. O(n*n)... doh!
	if( m_nextSlice == 0 )
		m_nextSlice = new Slice();
	SetupSlice( m_curSlice->m_seg + 1, m_nextSlice );

	{for( int i = 0; i < m_curSlice->m_nodes.GetSize(); i++ )
	{
		const Node& node1 = m_curSlice->m_nodes[i];
		if( node1.m_from < 0 )
			continue;

		const Seg&	seg1 = m_pTrack->GetAt(m_curSlice->m_seg);
		Vec2d		p1 = seg1.pt.GetXY() + seg1.norm.GetXY() * node1.m_offset;
		double		spd1 = node1.m_spd;

		double	mns, mxs, mxdy;
		m_cm.CalcSimuSpeedRanges(spd1, 3.0, 1.0, mns, mxs, mxdy);

		{for( int j = 0; j < m_nextSlice->m_nodes.GetSize(); j++ )
		{
			// work out time/speed for link...
			Node&		node2 = m_nextSlice->m_nodes[j];

			const Seg&	seg2 = m_pTrack->GetAt(m_nextSlice->m_seg);
			Vec2d		p2 = seg2.pt.GetXY() + seg2.norm.GetXY() * node2.m_offset;
			double		spd2 = node2.m_spd;

			double	dist = (p2 - p1).len();
			double	ang = node2.m_ang - node1.m_ang;
			NORM_PI_PI(ang);
			double	k1 = ang / dist;
            //double	k2 = Utils::CalcCurvatureTan(p1, node1.m_vel, p2);
            //double	K = GetAt(m_curSlice->m_seg).k;

			double	minSpd, maxSpd;
			m_cm.CalcSimuSpeeds(spd1, k1, dist, 1.0, minSpd, maxSpd);

			if( minSpd <= spd2 && spd2 <= maxSpd )
			{
				// got a candidate...
				double	deltaTime = 2 * dist / (spd1 + spd2);
				double	time = node1.m_time + deltaTime;
				if( node2.m_time == 0 || node2.m_time < time )
				{
					node2.m_time = time;
					node2.m_from = i;
				}
			}
		}}
	}}

	// remember from values...
	Trace&	trace = m_traces[m_nextSlice->m_seg];
	trace.m_from.SetSize(m_nextSlice->m_nodes.GetSize());
	{for( int i = 0; i < m_nextSlice->m_nodes.GetSize(); i++ )
	{
		trace.m_from[i] = m_nextSlice->m_nodes[i].m_from;
	}}

	// swap round slices...
	Slice*	pTemp = m_curSlice;
	m_curSlice = m_nextSlice;
	m_nextSlice = pTemp;
}
