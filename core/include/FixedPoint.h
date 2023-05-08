/*
 *  $Id: FixedPoint.h 10 2009-08-28 12:41:25Z matti $
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

#ifndef __FIXED_POINT_H
#define __FIXED_POINT_H

// FILE INFO
// This file holds several definitions concerning fixed point math.

#include "NovaTypes.h"

// number of fractional bits (precision) used in all fixed point calculations
const int_32 FixedPointPrec = 16;

// number '1' as fixed point
// fixed point '1'
const int_32 FixedPointOne = (1 << FixedPointPrec);

// fixed point '0.5'
const int_32 FixedPointHalf = FixedPointOne / 2;

// mask for fixed point number's fractional part (the lowest 16 bits)
const int_32 FixedPointFracMask = 0xffff;

// fixed point multiplier as a real
const real_64 FixedPointFactor = (real_64)FixedPointOne;

/** Swaps two integers. */
inline void Swap32( int_32& a, int_32& b )
{
    int_32 tmp = a;
    a = b;
    b = tmp;
}

#define MAX(a,b) ((a)>(b)?(a):(b))
#ifndef M_PI
#define M_PI       3.14159265358979323846
#endif

/**
 * Calculates the square root of an integer. 
 */
NOVA_IMPORT uint_32 FastSqrt( uint_32 n );

/**
 * Divides two fixed point integers, maintaining the precision.
 */
inline int_32 FixedLargeDiv( int_32 dividend, int_32 divisor )
{
    int_64 dividend_64 = dividend;
    int_64 divisor_64 = divisor;

    dividend_64 <<= FixedPointPrec;
    dividend_64 /= divisor_64;

    return (int_32)dividend_64;
}

/**
 * Converts a fixed point number into a real_64
 */
inline real_64 FixedToReal( int_32 fixed ) 
{
    return (real_64)(fixed / FixedPointFactor);
}

/**
 * Converts a real_64 into a fixed point number
 */
inline int_32 RealToFixed( real_64 real ) 
{
    return (int_32)(real * FixedPointFactor);
}

/**
 * Rounds a (positive) fixed point number to integer.
 */
inline int_32 RoundFixed( int_32 fixed )
{
    return ((fixed + FixedPointHalf) >> FixedPointPrec);
}

/**
 * Rounds a (positive) fixed point number down to the next integer.
 */
inline int_32 FloorFixed( int_32 fixed )
{
    return (fixed >> FixedPointPrec);
}

/**
 * Rounds a (positive) fixed point number up to the next integer.
 */
inline int_32 CeilFixed( int_32 fixed )
{
    return (FloorFixed( fixed ) + 1);
}

/**
 * Extracts the fractional part of a fixed point number.
 */
inline int_32 FracPartFixed( int_32 fixed ) 
{
    return (fixed & FixedPointFracMask);
}

#include "FixedOperations.h"

#endif
