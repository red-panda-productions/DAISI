#include "TrackPosition.h"


TrackPosition::TrackPosition(bool p_offroad, float p_toStart, float p_toRight, float p_toMiddle, float p_toLeft)
{
	m_offroad = p_offroad;
	m_toStart = p_toStart;
	m_toRight = p_toRight;
	m_toMiddle = p_toMiddle;
	m_toLeft = p_toLeft;
}

bool TrackPosition::Offroad() const
{
	return m_offroad;
}

float TrackPosition::ToStart() const
{
	return m_toStart;
}

float TrackPosition::ToRight() const
{
	return m_toRight;
}

float TrackPosition::ToMiddle() const
{
	return m_toMiddle;
}

float TrackPosition::ToLeft() const
{
	return m_toLeft;
}