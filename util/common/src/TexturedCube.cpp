/*
 *  $Id: ColorCube.h 17 2009-09-02 09:03:47Z matti $
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

#include "TexturedCube.h"
#include "NovaErrors.h"
#include "novalogging.h"

namespace nova3d {

NOVA_EXPORT TexturedCube::TexturedCube( NovaPixelFormat pixelFormat, 
					real_64 radius )
    : ColorCube( pixelFormat, radius )
{
    int res = Create();
    if ( res != NovaErrNone )
    {
	//##TODO## dedicated exception instead
	throw res;
    }
}

NOVA_EXPORT TexturedCube::~TexturedCube()
{
    delete m_texture;
}

// texture side length
const int TextureSide = 8;

int TexturedCube::Create()
{
    m_texture = new NO_THROW Texture();
    if ( m_texture == NULL )
    {
	return NovaErrNoMemory;
    }

    uint_32 palette[Texture::NumPaletteEntries];
    uint_8 data[TextureSide * TextureSide];

    // black and white
    palette[0] = nova3d::CreateColor( m_pixelFormat, 
				      0, 0, 0 );
    palette[1] = nova3d::CreateColor( m_pixelFormat, 
				      255, 255, 255 );

    // shades of blue
    for ( int i = 0; i < 2*TextureSide; i++ ) 
    {
	palette[i + 2] = nova3d::CreateColor( m_pixelFormat, 
					      0, 0, i * 250 / (2*TextureSide) );
    }

    uint_8* p = data;
    for ( int v = 0; v < TextureSide; v++ )
    {
	for ( int u = 0; u < TextureSide; u++ )
	{
	    // create a checkers pattern
	    uint_8 index = (uint_8)(v & 0x01);
	    if ( (u & 0x01) == 0 )
	    {
		index = (index + 1) & 0x01;
	    }

	    if ( index == 0 )
	    {
		// use shades of blue in the "black" squares
		index = u + v + 2;
	    }

	    *p++ = index;
	}
    }

    // create the texture with the generated palette and data
    int ret = m_texture->Create( m_pixelFormat, TextureSide, TextureSide, 
				 palette, data );
    if ( ret != NovaErrNone )
    {
	return ret;
    }

    // assign the created texture to all polygons
    SetTexture( m_texture );

    int_32 texcoords[12 * 3 * 2] = 
	{ 
	    0, 0, // poly 0
	    0, TextureSide, 
	    TextureSide, 0, 
	    TextureSide, 0, // poly 1
	    0, TextureSide, 
	    TextureSide, TextureSide,
	    0, 0, // poly 2
	    0, TextureSide, 
	    TextureSide, 0,
	    TextureSide, 0, // poly 3 
	    0, TextureSide, 
	    TextureSide, TextureSide,
	    0, 0, // poly 4
	    0, TextureSide, 
	    TextureSide, 0,
	    TextureSide, 0, // poly 5 
	    0, TextureSide, 
	    TextureSide, TextureSide,
	    0, 0, // poly 6
	    0, TextureSide, 
	    TextureSide, 0,
	    TextureSide, 0, // poly 7 
	    0, TextureSide, 
	    TextureSide, TextureSide,
	    0, 0, // poly 8
	    0, TextureSide, 
	    TextureSide, 0,
	    TextureSide, 0, // poly 9 
	    0, TextureSide, 
	    TextureSide, TextureSide,
	    0, 0, // poly 10
	    0, TextureSide, 
	    TextureSide, 0,
	    TextureSide, 0, // poly 11
	    0, TextureSide, 
	    TextureSide, TextureSide 
	};

    SetTextureCoordinates( texcoords );

    return NovaErrNone;
}

}; // namespace
