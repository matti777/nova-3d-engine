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
#ifdef __WINS__
    //=============================================
    // EMULATOR CODE: C
    //=============================================
    register uint_32 root = 0, guess = 0;

#define FASTSQRT_INNER(N) \
    guess = root + (1 << (N)); \
    if( n >= (guess << (N)) )   \
    {   n -= (guess << (N));   \
        root |= (2 << (N)); \
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
#else
    //=============================================
    // DEVICE CODE: ARM assembler
    //=============================================
    uint_32 root;
    asm volatile ( "LDR r0, %0" : : "m"(n) : "r0" ); // r0=N
    asm volatile ( "MOV r1, #1073741824" : : : "r1" ); // r1=1 << 30
    asm volatile ( "MOV r2, #-1073741824" : : : "r2" ); // r2=3 << 30

    // i = 0
    asm volatile ( "CMP r0, r1" ); 
    asm volatile ( "SUBHS r0, r0, r1" : : : "r0" );
    asm volatile ( "ADC r1, r2, r1, LSL #1" : : : "r1" );

    // i = 1
    asm volatile ( "CMP r0, r1, ROR #2" ); 
    asm volatile ( "SUBHS r0, r0, r1, ROR #2" : : : "r0" );
    asm volatile ( "ADC r1, r2, r1, LSL #1" : : : "r1" );

    // i = 2
    asm volatile ( "CMP r0, r1, ROR #4" ); 
    asm volatile ( "SUBHS r0, r0, r1, ROR #4" : : : "r0" );
    asm volatile ( "ADC r1, r2, r1, LSL #1" : : : "r1" );

    // i = 3
    asm volatile ( "CMP r0, r1, ROR #6" ); 
    asm volatile ( "SUBHS r0, r0, r1, ROR #6" : : : "r0" );
    asm volatile ( "ADC r1, r2, r1, LSL #1" : : : "r1" );

    // i = 4
    asm volatile ( "CMP r0, r1, ROR #8" ); 
    asm volatile ( "SUBHS r0, r0, r1, ROR #8" : : : "r0" );
    asm volatile ( "ADC r1, r2, r1, LSL #1" : : : "r1" );

    // i = 5
    asm volatile ( "CMP r0, r1, ROR #10" ); 
    asm volatile ( "SUBHS r0, r0, r1, ROR #10" : : : "r0" );
    asm volatile ( "ADC r1, r2, r1, LSL #1" : : : "r1" );

    // i = 6
    asm volatile ( "CMP r0, r1, ROR #12" ); 
    asm volatile ( "SUBHS r0, r0, r1, ROR #12" : : : "r0" );
    asm volatile ( "ADC r1, r2, r1, LSL #1" : : : "r1" );

    // i = 7
    asm volatile ( "CMP r0, r1, ROR #14" ); 
    asm volatile ( "SUBHS r0, r0, r1, ROR #14" : : : "r0" );
    asm volatile ( "ADC r1, r2, r1, LSL #1" : : : "r1" );

    // i = 8
    asm volatile ( "CMP r0, r1, ROR #16" ); 
    asm volatile ( "SUBHS r0, r0, r1, ROR #16" : : : "r0" );
    asm volatile ( "ADC r1, r2, r1, LSL #1" : : : "r1" );

    // i = 9
    asm volatile ( "CMP r0, r1, ROR #18" ); 
    asm volatile ( "SUBHS r0, r0, r1, ROR #18" : : : "r0" );
    asm volatile ( "ADC r1, r2, r1, LSL #1" : : : "r1" );

    // i = 10
    asm volatile ( "CMP r0, r1, ROR #20" ); 
    asm volatile ( "SUBHS r0, r0, r1, ROR #20" : : : "r0" );
    asm volatile ( "ADC r1, r2, r1, LSL #1" : : : "r1" );

    // i = 11
    asm volatile ( "CMP r0, r1, ROR #22" ); 
    asm volatile ( "SUBHS r0, r0, r1, ROR #22" : : : "r0" );
    asm volatile ( "ADC r1, r2, r1, LSL #1" : : : "r1" );

    // i = 12
    asm volatile ( "CMP r0, r1, ROR #24" ); 
    asm volatile ( "SUBHS r0, r0, r1, ROR #24" : : : "r0" );
    asm volatile ( "ADC r1, r2, r1, LSL #1" : : : "r1" );

    // i = 13
    asm volatile ( "CMP r0, r1, ROR #26" ); 
    asm volatile ( "SUBHS r0, r0, r1, ROR #26" : : : "r0" );
    asm volatile ( "ADC r1, r2, r1, LSL #1" : : : "r1" );

    // i = 14
    asm volatile ( "CMP r0, r1, ROR #28" ); 
    asm volatile ( "SUBHS r0, r0, r1, ROR #28" : : : "r0" );
    asm volatile ( "ADC r1, r2, r1, LSL #1" : : : "r1" );

    // i = 15
    asm volatile ( "CMP r0, r1, ROR #30" ); 
    asm volatile ( "SUBHS r0, r0, r1, ROR #30" : : : "r0" );
    asm volatile ( "ADC r1, r2, r1, LSL #1" : : : "r1" );

    asm volatile ( "BIC r1, r1, #-1073741824" : : : "r1" );
    asm volatile ( "STR r1, %0" : "=m"(root) );
    
    return root;
#endif
    }

