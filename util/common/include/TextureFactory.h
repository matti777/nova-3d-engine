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

#ifndef __TEXTUREFACTORY_H
#define __TEXTUREFACTORY_H

#include "Texture.h"

namespace nova3d {

// represents a color-frequency mapping for color reduction
struct ColorTableEntry
{
    uint_32 m_originalColor;
    uint_32 m_frequency;
};

/**
 * Utility for creating textures.<p />
 *
 * @author Matti Dahlbom
 * @version $Revision$
 */
class TextureFactory
{
 public: // Constructors and destructor
    TextureFactory();

 public: // New methods (Public API)
    /** 
     * Creates a texture from the given data, which MUST be in 888 pixel
     * format (3 bytes per pixel, size width * height * 3).<p />
     *
     * @param pixelFormat pixel format in which the textures palette
     *                    is to be created
     * @param width texture width
     * @param height texture height
     * @param pixelData pointer to the texture image's data
     * @param texture will be initialized to hold the texture object. Must
     *                be null when calling this method.
     * @return Nova error code or NovaErrNone if successful
     */
    NOVA_IMPORT int CreateTexture( NovaPixelFormat pixelFormat,
				   int width, int height, 
				   uint_8* pixelData, Texture*& texture );

 private: // New methods
    void PopulateColorTable( int numPixels, uint_8* data );
    int CountUniqueColors();
    void SortColorTable( int left, int right );
    void CreatePalette( NovaPixelFormat pixelFormat, 
			uint_32* palette, int numColors );
    void CreateData( NovaPixelFormat pixelFormat, 
		     uint_8* data, uint_8* originalData,
		     const uint_32* palette, int numPixels, 
		     int numColors );

 private: // Data
    // size of color table (max amount of colors in 555 pixel format)
    static const int ColorTableSize = 32768;

    // bitmask for masking indexes to the color table
    static const int ColorTableIndexMask = 0x7fff;

    // color frequency table
    ColorTableEntry m_colorTable[ColorTableSize];
};

}; // namespace

#endif
