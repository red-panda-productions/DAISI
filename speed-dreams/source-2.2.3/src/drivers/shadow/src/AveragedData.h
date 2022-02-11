#ifndef _AVERAGEDATA_H_
#define _AVERAGEDATA_H_

class AveragedData  
{
public:
	AveragedData( int nBinsX, double minX, double maxX,
				  int nBinsY, double minY, double maxY );
	~AveragedData();

	int		GetXSize() const;
	int		GetYSize() const;
	double	GetAxisValue( int axis, int index ) const;
	double	GetValueAt( int x, int y ) const;
	void	AddValue( double x, double y, double value );

private:
	struct Avg
	{
		Avg() : value(0), count(0) {}

		double	value;
		int		count;
	};

private:
	int		m_xSize;
	double	m_xMin;
	double	m_xSpan;
	int		m_ySize;
	double	m_yMin;
	double	m_ySpan;
	Avg*	m_pData;
};

#endif
