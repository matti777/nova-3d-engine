/*
 *  $Id: Camera.cpp 21 2009-09-08 11:12:30Z matti $
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

#include <string.h>
#include <stdlib.h>

#include "TextureFactory.h"
#include "NovaErrors.h"
#include "Display.h"
#include "novalogging.h"

namespace nova3d {

TextureFactory::TextureFactory()
{
}

// reads a 888 pixel format color of 3 bytes and increases the pointer
uint_32 Read3byteColor( uint_8*& data )
{
    uint_32 red = (uint_32)*data++;
    uint_32 green = (uint_32)*data++;
    uint_32 blue = (uint_32)*data++;

    return (red << 16) + (green << 8) + blue;
}

void TextureFactory::PopulateColorTable( int numPixels, uint_8* data )
{
    // clear the color table to all zeros
    memset( &m_colorTable, 0, sizeof( m_colorTable ) );

    uint_8* color = data;

    // check every pixel in the image
    for ( int i = 0; i < numPixels; i++ )
    {
	uint_32 color888 = Read3byteColor( color );
	uint_32 color555 = nova3d::ConvertColor( color888, PixelFormat888, 
						 PixelFormat555 );
	ColorTableEntry* entry = 
	    &(m_colorTable[color555 & ColorTableIndexMask]);
	entry->m_originalColor = color888;
	entry->m_frequency++;
    }
}

int TextureFactory::CountUniqueColors()
{
    int uniqueColors = 0;
    for ( int i = 0; i < ColorTableSize; i++ )
    {
	if ( m_colorTable[i].m_frequency > 0 )
	{
	    uniqueColors++;
	}
    }

    return uniqueColors;
}

void TextureFactory::SortColorTable( int left, int right )
{
    int qleft = left;
    int qright = right;
    uint_32 qpivot = m_colorTable[(qleft + qright) / 2].m_frequency;

    do 
    {
	while ( (m_colorTable[qleft].m_frequency > qpivot) && (qleft < right) )
	{
	    qleft++;
	}
        
	while ( (qpivot > m_colorTable[qright].m_frequency) && (qright > left) )
	{
	    qright--;
	}
	
	if ( qleft <= qright ) 
	{
	    // swap elements 
// 	    ScreenPolygon *tmp = *(m_visibleFaceList + qright);
// 	    *(m_visibleFaceList + qright) = *(m_visibleFaceList + qleft);
// 	    *(m_visibleFaceList + qleft) = tmp;
	    uint_32 color = m_colorTable[qright].m_originalColor;
	    uint_32 frequency = m_colorTable[qright].m_frequency;
	    m_colorTable[qright].m_originalColor = 
		m_colorTable[qleft].m_originalColor;
	    m_colorTable[qright].m_frequency = 
		m_colorTable[qleft].m_frequency;
	    m_colorTable[qleft].m_originalColor = color;
	    m_colorTable[qleft].m_frequency = frequency;
	    qleft++;
	    qright--;
	}
    } while ( qleft <= qright );
    
    // recursively sort left side
    if ( left < qright ) 
    {
	SortColorTable( left, qright );
    }

    // recursively sort right side
    if ( qleft < right ) 
    {
	SortColorTable( qleft, right );
    }
}

void TextureFactory::CreatePalette( NovaPixelFormat pixelFormat, 
				    uint_32* palette, int numColors )
{
    int paletteIndex = 0;

    // create the palette out of the 'numColors' first entries with >0 freq
    for ( int i = 0; (i < ColorTableSize) && (paletteIndex < numColors); i++ )
    {
	if ( m_colorTable[i].m_frequency > 0 )
	{
	    palette[paletteIndex++] = m_colorTable[i].m_originalColor;
	}
    }
}

void TextureFactory::CreateData( NovaPixelFormat pixelFormat, 
				 uint_8* data, uint_8* originalData, 
				 const uint_32* palette, 
				 int numPixels, int numColors )
{
    uint_8* fromPixel = originalData;
    uint_8* toPixel = data;

    // find closest match for every pixel from the given palette
    for ( int i = 0; i < numPixels; i++ )
    {
	uint_32 color = Read3byteColor( fromPixel );
	
	int paletteIndex = 0;
	uint_32 minDiff = MaxUint32;

	int red0, green0, blue0;
	nova3d::SplitColor( PixelFormat888, color, red0, green0, blue0 );

	for ( int j = 0; j < numColors; j++ )
	{
	    int red1, green1, blue1;
	    nova3d::SplitColor( pixelFormat, palette[j], red1, green1, blue1 );

            uint_32 diff = 
		((red0 - red1) * (red0 - red1)) + 
		((green0 - green1) * (green0 - green1)) + 
		((blue0 - blue1) * (blue0 - blue1));

            // check for exact match
            if ( diff == 0 ) 
	    {
                paletteIndex = j;
                break;
            } 
	    else if ( diff < minDiff ) 
	    {
                paletteIndex = j;
                minDiff = diff;
            }                
	}
	
	*toPixel++ = (uint_8)paletteIndex;
    }
}

NOVA_EXPORT int TextureFactory::CreateTexture( NovaPixelFormat pixelFormat,
					       int width, int height, 
					       uint_8* pixelData, 
					       Texture*& texture )
{
    LOG_DEBUG_F("TextureFactory::CreateTexture(): %d x %d", width, height);

    if ( texture != NULL )
    {
	return NovaErrInvalidArgument;
    }

    // insert colors into the color table in 555-pixelformat 
    // (reduce their amount to a maximum of 32768)
    PopulateColorTable( width * height, pixelData );

    // calculate the colors in the table
    int numUniqueColors = CountUniqueColors();
    LOG_DEBUG_F("TextureFactory::CreateTexture(): %d unique colors", 
		numUniqueColors);
    
    // see if we must reduce the color count
    if ( numUniqueColors > Texture::NumPaletteEntries )
    {
	SortColorTable( 0, ColorTableSize - 1 );
	numUniqueColors = Texture::NumPaletteEntries;
    }

    // create the palette
    uint_32 palette[Texture::NumPaletteEntries];
    CreatePalette( pixelFormat, palette, numUniqueColors );

    // create the indexed texture bitmap data
    uint_8* data = (uint_8*)malloc( width * height * sizeof(uint_8) );
    if ( data == NULL )
    {
	return NovaErrNoMemory;
    }
    CreateData( pixelFormat, data, pixelData, palette, 
		width * height, numUniqueColors );

    // create the texture
    texture = new Texture();
    int ret = texture->Create( pixelFormat, width, height, 
			       palette, data );

    // cleanup
    free( data );
    
    return ret;
}

}; // namespace
