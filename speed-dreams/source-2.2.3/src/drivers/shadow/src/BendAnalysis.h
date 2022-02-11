#ifndef _BENDANALYSIS_H_
#define _BENDANALYSIS_H_

#include "Array.h"

class LinePath;
class MyTrack;

class BendAnalysis  
{
public:
	enum // types
	{
		T_UNKNOWN,
		T_STRAIGHT,
		T_LEFT,
		T_RIGHT,
	};

	struct BendInfo
	{
		int		m_type;		// left-bend, right-bend, straight
		int		m_subType;	// straight-into-left, straight-into-right,
							// left-entry, left-exit,
							// right-entry, right-exit
		double	m_start;	// seg of start of this bend.
		double	m_length;	// nSegs in length of bend.
		double	m_subLength;

		double	m_maxK;		// max(fabs(K)) of this bend.
	};

	struct SegInfo
	{
		int		m_id;
		double	m_distToNext;
	};

public:
	BendAnalysis( const LinePath& path, const MyTrack& track );
	~BendAnalysis();

	int				GetSize() const;
	const BendInfo&	GetAt( int index ) const;

private:
	const MyTrack&	m_track;

	SegInfo*			m_pSegInfo;
	Array<BendInfo>		m_bendInfo;
};

#endif
