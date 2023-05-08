/*
 *  $Id: List.h 23 2009-09-10 13:57:16Z matti $
 *
 *  Nova 3D Engine - A portable object oriented, scene graph based, 
 *  lightweight real-time 3D software rendering framework. 
 *  Copyright (C) 2001-2009 Matti Dahlbom
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Contact: Matti Dahlbom <matti at 777-team dot org>
 */

#ifndef __LIST_H
#define __LIST_H

#include <string.h>
#include <stdlib.h>

#include "NovaErrors.h"
#include "novalogging.h"

namespace nova3d {

// list default granularity
const int ListDefaultGranularity = 4;

/**
 * A templated class to provide a minimalistic std::vector like behaviour.<p />
 * 
 * @author Matti Dahlbom
 * @version $Revision: 23 $
 */
template <class T> class List 
{
 public: // Constructors and destructor
    List<T>( int granularity = ListDefaultGranularity );
    virtual ~List();
        
 public: // New methods
    /**
     * Appends a new entry in the end of this list.<p />
     */
    int Append( T& t );
        
    /**
     * Removes an entry at a given index from this list.<p />
     */
    int Remove( int index );
        
    /**
     * Returns the number of elements in the list currently. Note that
     * calling Remove() on this list makes the result returned by an
     * earlier call invalid.<p />
     */
    int Count() const;
        
    /**
     * Returns the entry at the given index in the list. Note that
     * calling Remove() on this list makes the result returned by an
     * earlier call invalid.<p />
     */        
    int Get( int index, T& t ) const;
        
    /**
     * Returns a pointer to the entry at the given index in the list. Note that
     * calling Remove() on this list makes the result returned by an
     * earlier call invalid.<p />
     */        
    int Get( int index, T*& t ) const;
        
    /** 
     * Resets the list, removing all the elements and reseting the maximum
     * size to one times m_granularity.
     */
    int Reset();
        
 private: // Data
    int m_numElements;
    int m_maxElements;
    int m_granularity;
    T* m_data; 
};

// method implementations

template <class T> List<T>::List( int granularity )
: m_numElements( 0 ),
    m_maxElements( 0 ),
    m_granularity ( granularity ),
    m_data( NULL )
{
}

template <class T> List<T>::~List()
{
    if ( m_data != NULL )
    {
	free( m_data );
	m_data = NULL;
    }
}

template <class T> int List<T>::Append( T& t )
{
    // check if need to reallocate
    if ( m_numElements == m_maxElements )
    {
        m_maxElements += m_granularity;
	void* p = realloc( m_data, sizeof(T) * m_maxElements );
	if ( p == NULL )
	{
	    // failed to allocate more memory
	    return NovaErrNoMemory;
	}
	else
	{
	    m_data = (T*)p;
	}
    }
    
    // append the new element
    memcpy( &(m_data[m_numElements]), &t, sizeof(T) );
    m_numElements++;
    
    return NovaErrNone;
}

template <class T> int List<T>::Remove( int index )
{
    if ( (index < 0) || (index >= m_numElements) )
    {
        return NovaErrOutOfBounds;
    }
    
    if ( index < (m_numElements - 1) )
    {
        // move the remaining entries up one slot
        memmove( &m_data[index], &m_data[index + 1], 
                 sizeof(T) * (m_numElements - index - 1) );
    }
    
    m_numElements--;
    
    return NovaErrNone;
}

template <class T> int List<T>::Count() const
{
    return m_numElements;
}

template <class T> int List<T>::Get( int index, T& t ) const
{
    if ( (index < 0) || (index >= m_numElements) )
    {
        return NovaErrOutOfBounds;
    }
    else
    {
        t = *(m_data + index);
        return NovaErrNone;
    }
}

template <class T> int List<T>::Get( int index, T*& t ) const
{
    if ( (index < 0) || (index >= m_numElements) )
    {
        return NovaErrOutOfBounds;
    }
    else
    {
        t = (m_data + index);
        return NovaErrNone;
    }    
}

template <class T> int List<T>::Reset() 
{
    m_numElements = 0;
    
    return NovaErrNone;
}

}; // namespace

#endif
