/*
 *  $Id: Texture.cpp 23 2009-09-10 13:57:16Z matti $
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

#include <stdlib.h>
#include <string.h>

#include "Texture.h"
#include "FixedPoint.h"
#include "NovaErrors.h"

namespace nova3d {

// max texture height/width
const int MaxTextureSidePower = 10;
const int MaxTextureSide = 1 << MaxTextureSidePower; // 1024;

NOVA_EXPORT Texture::Texture()
    : m_width( 0 ),
      m_height( 0 ),
      m_shift( 0 ), 
      m_umask( 0 ), 
      m_vmask( 0 ),
      m_pixelFormat( PixelFormatUndefined ), 
      m_numPalettes( 0 ), 
      m_numPalettesShift( 0 ),
      m_palette( NULL ),
      m_data( NULL )      
{
}

NOVA_EXPORT Texture::~Texture()
{
    free( m_palette );
    free( m_data );
}

NOVA_EXPORT int Texture::Create( NovaPixelFormat pixelFormat, 
                                 int width, int height,
                                 uint_32* palette, uint_8* data )
{
    if ( (width > MaxTextureSide) || (height > MaxTextureSide) )
    {
        return NovaErrTextureTooLarge;
    }
    
    m_umask = 0xffffffffu;
    m_vmask = 0xffffffffu;
    
    // find out the u mask
    for ( int i = 0; i < MaxTextureSidePower; i++ )
    {
        if ( (1 << i) == width )
	{
            m_umask = width - 1;
	    m_shift = i;
            break;
	}
    }

    // find out the v mask
    for ( int i = 0; i < MaxTextureSidePower; i++ )
    {
        if ( (1 << i) == height )
	{
            m_vmask = height - 1;
            break;
	}
    }

    // texture width must be a power of 2; check this
    if ( m_umask == 0xffffffffu )
    {
	m_vmask = 0xffffffffu;
	return NovaErrTextureDimensionInvalid;
    }
    
    // make a copy of the palette 
    free( m_palette );
    m_palette = NULL;
    m_palette = (uint_32*)malloc( NumPaletteEntries * sizeof(uint_32) );
    if ( m_palette == NULL )
    {
	return NovaErrNoMemory;
    }
    memcpy( m_palette, palette, NumPaletteEntries * sizeof(uint_32) );

    // make a copy of the data
    free( m_data );
    m_data = NULL;
    m_data = (uint_8*)malloc( width * height * sizeof(uint_8) );
    if ( m_data == NULL )
    {
	return NovaErrNoMemory;
    }
    memcpy( m_data, data, width * height * sizeof(uint_8) );

    // set up texture properties. the texture initially has a single palette.
    m_numPalettes = 1;
    m_pixelFormat = pixelFormat;
    m_width = width;
    m_height = height;
    
    return NovaErrNone;
}

NOVA_EXPORT int Texture::CreateLinearPalettes( real_64 gain )
{
    int fixedGain = ::RealToFixed( gain );
   
    // decide the number of palettes based on the pixel format
    int numPalettes;
    switch ( m_pixelFormat ) 
    {
    case PixelFormat444:
	m_numPalettesShift = 4;
	numPalettes = 16;
	break;
    case PixelFormat555:
	m_numPalettesShift = 5;
	numPalettes = 32;
	break;
    case PixelFormat565:
    case PixelFormat666:
	m_numPalettesShift = 6;
	numPalettes = 64;
	break;
    case PixelFormat888:	
	m_numPalettesShift = 8;
	numPalettes = 256;
	break;
    case PixelFormatUndefined:
	return NovaErrNotFound;
	break;
    }
   
    // allocate memory for the new palettes
    uint_32* newPalettes = 
	(uint_32*)malloc( NumPaletteEntries * numPalettes * sizeof(uint_32) );
    if ( newPalettes == NULL )
    {
	return NovaErrNoMemory;
    }

    // calculate the interpolation values
    int_32 factor = 0;
    int_32 slope = fixedGain >> m_numPalettesShift;

    // linearly create the palettes
    uint_32* newColor = newPalettes;    
    int red, green, blue;

    for ( int i = 0; i < m_numPalettes; i++ ) 
    {
	uint_32* oldColor = m_palette;

	for ( int j = 0; j < NumPaletteEntries; j++ ) 
	{
	    nova3d::SplitColorIntoFixedPoint( m_pixelFormat, *oldColor++, 
					      red, green, blue );
	    nova3d::ScaleColor( m_pixelFormat, red, green, blue, factor );
	    *newColor++ = nova3d::CreateColor( m_pixelFormat , 
					       red >> FixedPointPrec, 
					       green >> FixedPointPrec, 
					       blue >> FixedPointPrec );
	}

	factor += slope;
    }

    // deallocate old palette and apply the new one
    free( m_palette );
    m_palette = newPalettes;

    return NovaErrNone;
}

void Texture::ScaleIntensity( int_32& intensity ) const
{
    int_32 maxIntensityFixed = (m_numPalettes - 1) << FixedPointPrec;
    intensity <<= m_numPalettesShift;
    if ( intensity > maxIntensityFixed ) 
    {
	intensity = maxIntensityFixed;
    }
}

}; // namespace
