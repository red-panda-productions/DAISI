/***************************************************************************

    created              : Sat Mar 18 23:16:38 CET 2006
    copyright            : (C) 2006 by Tim Foden (c) by 2015 Xavier Bertaux
    email                : bertauxx@yahoo.fr
    version              : $Id: Array.h 5522 2015-05-09 21:03:25Z torcs-ng $

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef _ARRAY_H_
#define _ARRAY_H_

#include <memory.h>

template<class T> class Array  
{
public:
	Array()
	:	m_size(0),
		m_allocated(0),
		m_pData(0)
	{
	}

	Array( const Array& a )
	:	m_size(0),
		m_allocated(0),
		m_pData(0)
	{
		RemoveAll();
		for( int i = 0; i < a.m_size; i++ )
			Add( a.m_pData[i] );
	}

	virtual ~Array()
	{
		delete [] m_pData;
	}

	Array&		operator=( const Array& a )
	{
		RemoveAll();
		for( int i = 0; i < a.m_size; i++ )
			Add( a.m_pData[i] );
		return *this;
	}

	void		RemoveAll()
	{
		m_size = 0;
	}

	void		SetSize( int size )
	{
		AdjustForSize( size );
		m_size = size;
	}

	void		Add( const T& item )
	{
		AdjustForSize( m_size + 1 );
		m_pData[m_size++] = item;
	}

	int			GetSize() const
	{
		return m_size;
	}

	bool		IsEmpty() const
	{
		return m_size == 0;
	}

	void		InsertAt( int index, const T& item )
	{
		AdjustForSize( m_size + 1 );
		for( int i = m_size; i > index; i-- )
			m_pData[i] = m_pData[i - 1];
		m_pData[index] = item;
		m_size++;
	}

	void		RemoveAt( int index )
	{
		m_size--;
		for( int i = index; i < m_size; i++ )
			m_pData[i] = m_pData[i + 1];
	}

	const T&	GetAt( int index ) const
	{
		return m_pData[index];
	}

	T&			GetAt( int index )
	{
		return m_pData[index];
	}

	const T&	operator[]( int index ) const
	{
		return m_pData[index];
	}

	T&			operator[]( int index )
	{
		return m_pData[index];
	}

	bool		operator==( const Array& a ) const
	{
		if( this == &a )
			return true;

		if( m_size != a.m_size )
			return false;

		for( int i = 0; i < m_size; i++ )
			if( m_pData[i] != a.m_pData[i] )
				return false;

		return true;
	}

	bool		operator!=( const Array& a ) const
	{
		return !operator==(a);
	}

private:
	void	AdjustForSize( int size )
	{
		if( size > m_allocated )
		{
			int	newAllocated = m_allocated * 2;
			if( newAllocated < 4 )
				newAllocated = 4;
			if( newAllocated < size )
				newAllocated = size;

			T*	pNewData = new T[newAllocated];
			if( m_size )
				memcpy( pNewData, m_pData, sizeof(T) * m_size );
			delete [] m_pData;
			m_pData = pNewData;
			m_allocated = newAllocated;
		}
	}

private:
	int		m_size;
	int		m_allocated;
	T*		m_pData;
};

#endif
