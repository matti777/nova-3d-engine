/*
 *  $Id: Display.h 22 2009-09-09 17:25:01Z matti $
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

#ifndef __DISPLAY_H
#define __DISPLAY_H

// FILE INFO
// This file contains constants and definitions for display (mode) related
// things as well as structures to describe visual (2D) entities.

#include "NovaTypes.h"

namespace nova3d {

// forward declarations
class Texture;

/** Display mode constants used by Nova3D */
enum NovaDisplayMode 
    {
	DisplayModeUndefined, 
	DisplayMode12,
	DisplayMode16,
	DisplayMode18,
	DisplayMode24
    };

/** Pixel formats supported by Nova3D */
enum NovaPixelFormat
    {
	PixelFormatUndefined, 
	PixelFormat444,
	PixelFormat555,
	PixelFormat565,
	PixelFormat666,
	PixelFormat888
    };

/** Represents a surface ("bitmap") that can be rendered to. */
struct RenderingCanvas 
{
    // canvas dimensions
    int_32 m_top;
    int_32 m_bottom;
    int_32 m_left;
    int_32 m_right;
    int_32 m_height;
    int_32 m_width;
    int_32 m_centerX;
    int_32 m_centerY;

    // number of pixels per scanline
    //    int_32 m_pixelsPerScanline;

    // pixel format for this canvas
    NovaPixelFormat m_pixelFormat;

    // number of bytes per scanline. this can be used to support drawing
    // to a subwindow of a bigger memory area (display memory for example).
    // thus this does *not* equal m_width  * (pixel size)
    int_32 m_bytesPerScanline;

    // canvas buffer memory address
    void* m_bufferPtr;
};

/**
 * Represents a (2D) vertex in a visible screen polygon about to be
 * rendered. All the data members are in fixed point.
 */
struct ScreenVertex 
{
    int_32 m_x;
    int_32 m_y;
    int_32 m_z;

    union 
    {
        struct 
	{
            int_32 m_red;
            int_32 m_green;
            int_32 m_blue;
	} m_color;
        struct 
	{
            int_32 m_u;
            int_32 m_v;
            int_32 m_intensity;
	} m_textureCoordinates;
    };
};

/**
 * Represents a transformed, projected and clipped, visible 
 * triangular polygon face.
 */
struct ScreenPolygon 
{
    ScreenVertex m_v1;
    ScreenVertex m_v2;
    ScreenVertex m_v3;
    int_32 m_zSortValue;

    // these are copied directly from the Shape.
    Texture* m_texture; 

    // these are copied directly from the Shape. the constants are
    // defined in Shape.h
    uint_32 m_polygonFlags; 
    
};

// util macros to pack three 8-bit color components (r,g,b) into 
// pixel formats.
// 0000 1111 = 0F
// 0001 1111 = 1F   
// 0011 1111 = 3F
// 1111 1111 = FF
#define PIXEL_444(r,g,b) (uint_32)(((r&0x0f)<<8) | ((g&0x0f)<<4) | (b&0x0f))
#define PIXEL_555(r,g,b) (uint_32)(((r&0x1f)<<10) | ((g&0x1f)<<5) | (b&0x1f))
#define PIXEL_565(r,g,b) (uint_32)(((r&0x1f)<<11) | ((g&0x3f)<<5) | (b&0x1f))
#define PIXEL_666(r,g,b) (uint_32)(((r&0x3f)<<12) | ((g&0x3f)<<6) | (b&0x3f))
#define PIXEL_888(r,g,b) (uint_32)(((r&0xff)<<16) | ((g&0xff)<<8) | (b&0xff))

/** 
 * Creates a packed color out of the R,G,B components in the given pixel format.
 */
NOVA_IMPORT uint_32 CreateColor( NovaPixelFormat pixelFormat, 
                                 int red, int green, int blue ); 

/** 
 * Splits a color in the given pixel format into R,G,B compontents in
 * the fixed point format (values left-shifted by FixedPointPrec).<p />
 */
NOVA_IMPORT void SplitColorIntoFixedPoint( NovaPixelFormat pixelFormat, 
					   uint_32 color, 
					   int_32& red, int_32& green, 
					   int_32& blue ); 

/** 
 * Splits a color in the given pixel format into R,G,B compontents.<p />
 */
NOVA_IMPORT void SplitColor( NovaPixelFormat pixelFormat, uint_32 color, 
			     int_32& red, int_32& green, int_32& blue ); 

/**
 * Converts a color value from one pixel format to another.<p />
 */
 NOVA_IMPORT uint_32 ConvertColor( uint_32 color, NovaPixelFormat fromFormat, 
				   NovaPixelFormat toFormat );

/**
 * Scales a color of 3 components (already in fixed point form) by a 
 * scaler value. The color values are capped to their respective 
 * maximum values.
 */
NOVA_IMPORT void ScaleColor( NovaPixelFormat pixelFormat, 
			     int_32& red, int_32& green, int_32& blue, 
			     int_32 scaler );

}; // namespace

#endif

