// BendAnalysis.cpp: implementation of the BendAnalysis class.
//
//////////////////////////////////////////////////////////////////////

#include "BendAnalysis.h"

#include "LinePath.h"
#include "MyTrack.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

BendAnalysis::BendAnalysis( const LinePath& path, const MyTrack& track )
:	m_track(track)
{
	const int NSEG = track.GetSize();

	double	maxK = 0;
	double	minK = 1;

    for( int i = 0; i < NSEG; i++ )
	{
		double	fabsK = fabs(path.GetAt(i).k);
		if( maxK < fabsK )
			maxK = fabsK;
		if( minK > fabsK )
			minK = fabsK;
    }

	if( minK == maxK )
	{
		// curvature is continuous around whole track... wierd.
		return;
	}

	double	STRAIGHT_K = 0.0005;
	if( maxK < STRAIGHT_K )
		STRAIGHT_K = minK + 0.05 * (maxK - minK);

	m_pSegInfo = new SegInfo[NSEG];
	memset( m_pSegInfo, 0, sizeof(SegInfo) * NSEG );

	int		seg = 0;
	int		endSeg = -1;
	int		lastType = T_UNKNOWN;
	int		curType = T_UNKNOWN;

	BendInfo	bi;
	bi.m_type		= T_UNKNOWN;
	bi.m_subType	= 0;
	bi.m_length		= 0;
	bi.m_subLength	= 0;
	bi.m_maxK		= maxK;

	while( seg != endSeg )
	{
		const LinePath::PathPt&	pathPt = path.GetAt(seg);

		double	fabsK = fabs(pathPt.k);
		if( maxK < fabsK )
			maxK = fabsK;

		int	type = T_UNKNOWN;
		if( fabsK < STRAIGHT_K )
			type = T_STRAIGHT;
		else if( pathPt.k > 0 )
			type = T_LEFT;
		else
			type = T_RIGHT;

		if( lastType == T_UNKNOWN )
		{
			lastType = type;
		}
		else if( lastType != type )
		{
			if( bi.m_type != T_UNKNOWN )
			{
				// must have completed bi to add.
				bi.m_subLength = seg - bi.m_start;
				if( bi.m_subLength < 0 )
					bi.m_subLength += NSEG;
				bi.m_maxK = maxK;
				m_bendInfo.Add( bi );
			}
			else
			{
				// 1st bend... setup last seg index.
				endSeg = seg;
			}

			// start new bi here.
			bi.m_type		= type;
			bi.m_subType	= 0;
			bi.m_start		= seg;
			bi.m_length		= 0;
			bi.m_subLength	= 0;
			bi.m_maxK		= 0;

			maxK = 0;
			lastType = type;
		}

		SegInfo&	si = m_pSegInfo[seg];
		si.m_id = m_bendInfo.GetSize() - 1;

		seg = (seg + 1) % NSEG;
	}
}

BendAnalysis::~BendAnalysis()
{
	delete [] m_pSegInfo;
}

int		BendAnalysis::GetSize() const
{
	return m_bendInfo.GetSize();
}

const BendAnalysis::BendInfo&	BendAnalysis::GetAt( int index ) const
{
	return m_bendInfo[index];
}
