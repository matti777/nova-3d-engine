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

#ifndef __FIXEDLARGEMUL_H
#define __FIXEDLARGEMUL_H

// FILE INFO
// This file contains the Symbian implementations of the fixed point 
// operations.

#include <e32base.h>
#include "NovaTypes.h"

/**
 * Multiplies two fixed point integers and shifts away the
 * extra precision.
 */
inline int_32 FixedLargeMul( int_32 multiplicand, int_32 multiplier )
    {
#ifdef __WINSCW__
    //=============================================
    // EMULATOR CODE: use TInt64
    //=============================================
    TInt64 multiplicand_64 = multiplicand;
    TInt64 multiplier_64 = multiplier;
    multiplicand_64 *= multiplier_64;
    multiplicand_64 >>= FixedPointPrec;
    
    return (int_32)multiplicand_64;
#else
    //=============================================
    // DEVICE CODE: use native 32x32->64 multiply
    //=============================================
    TInt ret;

    asm volatile ( "SMULL r6, r7, %0, %1" : : 
                   "r"(multiplicand), "r"(multiplier) : "r6", "r7" );
    asm volatile ( "MOV r6, r6, LSR #16" : : : "r6" );
    asm volatile ( "ORR r6, r6, r7, LSL #16" : : : "r6" );
    asm volatile ( "STR r6, %0" : "=m"(ret) );

    return ret;
#endif
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
#ifdef __WINSCW__
    //=============================================
    // EMULATOR CODE: use FixedLargeMul()'s
    //=============================================
    return (::FixedLargeMul( value1, value2 ) + 
            ::FixedLargeMul( value3, value4 ) + 
            ::FixedLargeMul( value5, value6 ));
#else
    //=============================================
    // DEVICE CODE: use native 32x32->64 mults
    //=============================================
    int_32 ret;

    asm volatile ( "LDR r2, %0" : : "m"(value1) : "r2" );
    asm volatile ( "LDR r3, %0" : : "m"(value2) : "r3" );
    asm volatile ( "LDR r4, %0" : : "m"(value3) : "r4" );
    asm volatile ( "LDR r5, %0" : : "m"(value4) : "r5" );
    asm volatile ( "LDR r6, %0" : : "m"(value5) : "r6" );
    asm volatile ( "LDR r7, %0" : : "m"(value6) : "r7" );
    asm volatile ( "SMULL r8, r9, r2, r3" : : : "r8", "r9" ); // r8,r9 = r2*r3
    asm volatile ( "SMLAL r8, r9, r4, r5" : : : "r8", "r9" ); // r8,r9 += r4*r5
    asm volatile ( "SMLAL r8, r9, r6, r7" : : : "r8", "r9" ); // r8,r9 += r6*r7
    asm volatile ( "MOV r8, r8, LSR #16" : : : "r8" ); // r8 >>= 16
    asm volatile ( "ORR r8, r8, r9, LSL #16" : : : "r8" ); // r8 = r8 | (r9 << 16)
    asm volatile ( "STR r8, %0" : "=m"(ret) );

    return ret;
#endif
    }

#endif
