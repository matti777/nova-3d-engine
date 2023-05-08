/*
 *  $Id: Camera.cpp,v 1.42 2005/03/20 18:26:48 matti Exp $
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

#include "FixedPoint.h"

// The algorithm used here was designed by 
// Wilco Dijkstra, and was posted to comp.sys.arm in 1996.
NOVA_EXPORT uint_32 FastSqrt( uint_32 n )
{
  register uint_32 root = 0, guess = 0;

#define FASTSQRT_INNER(N)			\
  guess = root + (1 << (N));			\
  if( n >= (guess << (N)) )			\
  {   n -= (guess << (N));			\
    root |= (2 << (N));				\
  }

  FASTSQRT_INNER( 15 );
  FASTSQRT_INNER( 14 );
  FASTSQRT_INNER( 13 );
  FASTSQRT_INNER( 12 );
  FASTSQRT_INNER( 11 );
  FASTSQRT_INNER( 10 );
  FASTSQRT_INNER( 9 );
  FASTSQRT_INNER( 8 );
  FASTSQRT_INNER( 7 );
  FASTSQRT_INNER( 6 );
  FASTSQRT_INNER( 5 );
  FASTSQRT_INNER( 4 );
  FASTSQRT_INNER( 3 );
  FASTSQRT_INNER( 2 );
  FASTSQRT_INNER( 1 );
  FASTSQRT_INNER( 0 );

#undef FASTSQRT_INNER 

  return root >> 1;
}

