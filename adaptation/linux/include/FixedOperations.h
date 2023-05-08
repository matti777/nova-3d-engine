/*
 *  $Id: FixedOperations.h 19 2009-09-07 07:16:57Z matti $
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

#ifndef __FIXEDLARGEMUL_H
#define __FIXEDLARGEMUL_H

// FILE INFO
// This file contains the Linux implementations of the fixed point 
// operations.

#include "NovaTypes.h"

NOVA_IMPORT uint_32 FastSqrt( uint_32 n );

//##TODO## remove
#include <stdio.h>

/**
 * Multiplies two signed fixed point integers and shifts away the
 * extra precision.
 */
inline int_32 FixedLargeMul( int_32 multiplicand, int_32 multiplier )
{
  //##TODO## check that this works
  int_64 multiplicand_64 = multiplicand;
  int_64 multiplier_64 = multiplier;
  multiplicand_64 *= multiplier_64;
  multiplicand_64 >>= FixedPointPrec;
  
  if ( (multiplicand_64 > 2147483647LL) || (multiplicand_64 < -2147483648LL) )
  {
      printf("FixedLargeMul() overflow! = %d x %d\n", multiplicand, multiplier);
      throw multiplicand_64;
  }

  return (int_32)multiplicand_64;
}

/**
 * Multiplies 3 pairs of values together and adds the results together, ie:
 *
 * result = value1 * value2 + value3 * value4 + value5 * value6
 */
inline int_32 FixedTripleMul( int_32 value1, int_32 value2, 
                              int_32 value3, int_32 value4, 
                              int_32 value5, int_32 value6 ) 
{
  return (::FixedLargeMul( value1, value2 ) + 
	  ::FixedLargeMul( value3, value4 ) + 
	  ::FixedLargeMul( value5, value6 ));
}

#endif
